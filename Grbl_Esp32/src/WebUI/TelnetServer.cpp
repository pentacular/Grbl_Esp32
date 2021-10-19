/*
  TelnetServer.cpp -  telnet server functions class

  Copyright (c) 2014 Luc Lebosse. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "../Grbl.h"

#if defined(ENABLE_WIFI) && defined(ENABLE_TELNET)

#    include "WifiServices.h"

#    include "TelnetServer.h"
#    include "WifiConfig.h"
#    include <WiFi.h>

namespace WebUI {
    Telnet_Server telnet_server;
    bool          Telnet_Server::_setupdone    = false;
    uint16_t      Telnet_Server::_port         = 0;
    WiFiServer*   Telnet_Server::_telnetserver = NULL;
    WiFiClient    Telnet_Server::_telnetClients[MAX_TLNT_CLIENTS];

#    ifdef ENABLE_TELNET_WELCOME_MSG
    IPAddress Telnet_Server::_telnetClientsIP[MAX_TLNT_CLIENTS];
#    endif

    Telnet_Server::Telnet_Server() {
        reset_rx_buffer_read_position();
    }

    bool Telnet_Server::begin() {
        bool no_error = true;
        end();
        reset_rx_buffer_read_position();

        if (telnet_enable->get() == 0) {
            return false;
        }
        _port = telnet_port->get();

        //create instance
        _telnetserver = new WiFiServer(_port, MAX_TLNT_CLIENTS);
        _telnetserver->setNoDelay(true);
        String s = "[MSG:TELNET Started " + String(_port) + "]\r\n";
        grbl_send(CLIENT_ALL, (char*)s.c_str());
        //start telnet server
        _telnetserver->begin();
        _setupdone = true;
        return no_error;
    }

    void Telnet_Server::end() {
        _setupdone    = false;
        reset_rx_buffer_read_position();
        if (_telnetserver) {
            delete _telnetserver;
            _telnetserver = NULL;
        }
    }

    void Telnet_Server::clearClients() {
        //check if there are any new clients
        if (_telnetserver->hasClient()) {
            uint8_t i;
            for (i = 0; i < MAX_TLNT_CLIENTS; i++) {
                //find free/disconnected spot
                if (!_telnetClients[i] || !_telnetClients[i].connected()) {
#    ifdef ENABLE_TELNET_WELCOME_MSG
                    _telnetClientsIP[i] = IPAddress(0, 0, 0, 0);
#    endif
                    if (_telnetClients[i]) {
                        _telnetClients[i].stop();
                    }
                    _telnetClients[i] = _telnetserver->available();
                    break;
                }
            }
            if (i >= MAX_TLNT_CLIENTS) {
                //no free/disconnected spot so reject
                _telnetserver->available().stop();
            }
        }
    }

    // Note: this operation may drop data, as it only reports the amount written
    // to the last client.
    size_t Telnet_Server::write(const uint8_t* buffer, size_t size) {
        size_t wsize = 0;
        if (!_setupdone || _telnetserver == NULL) {
            log_d("[TELNET out blocked]");
            return 0;
        }

        clearClients();

        //log_d("[TELNET out]");
        //push UART data to all connected telnet clients
        for (uint8_t i = 0; i < MAX_TLNT_CLIENTS; i++) {
            if (_telnetClients[i] && _telnetClients[i].connected()) {
                //log_d("[TELNET out connected]");
                wsize = _telnetClients[i].write(buffer, size);
                COMMANDS::wait(0);
            }
        }
        return wsize;
    }

    void Telnet_Server::handle() {
        COMMANDS::wait(0);
        //check if can read
        if (!_setupdone || _telnetserver == NULL) {
            return;
        }
        clearClients();
        //check clients for data
        //uint8_t c;
        for (uint8_t i = 0; i < MAX_TLNT_CLIENTS; i++) {
            if (_telnetClients[i] && _telnetClients[i].connected()) {
#    ifdef ENABLE_TELNET_WELCOME_MSG
                if (_telnetClientsIP[i] != _telnetClients[i].remoteIP()) {
                    report_init_message(CLIENT_TELNET);
                    _telnetClientsIP[i] = _telnetClients[i].remoteIP();
                }
#    endif
                if (_telnetClients[i].available()) {
                    uint8_t buf[1024];
                    COMMANDS::wait(0);
                    int readlen  = _telnetClients[i].available();
                    int writelen = get_rx_buffer_writable_size();
                    if (readlen > writelen) {
                        readlen = writelen;
                    }
                    if (readlen > 0) {
                        // This looks be safe, but let's check the invariants.
                        if (_telnetClients[i].read(buf, readlen) != readlen) {
                          grbl_send(CLIENT_ALL, "QQ/Telnet/read: failed\r\n");
                        } else {
                          buf[readlen] = '\0';
                          grbl_send(CLIENT_ALL, "QQ/Telnet/read/begin\r\n");
                          grbl_send(CLIENT_ALL, (char *)buf);
                          grbl_send(CLIENT_ALL, "\r\nQQ/Telnet/read/end\r\n");
                        }
                        if (!push(buf, readlen)) {
                          grbl_send(CLIENT_ALL, "QQ/Telnet/push: failed\r\n");
                        }
                    }
                    return;
                }
            } else {
                if (_telnetClients[i]) {
#    ifdef ENABLE_TELNET_WELCOME_MSG
                    _telnetClientsIP[i] = IPAddress(0, 0, 0, 0);
#    endif
                    _telnetClients[i].stop();
                }
            }
            COMMANDS::wait(0);
        }
    }

    int Telnet_Server::peek(void) {
        if (get_rx_buffer_readable_size() >= 1) {
            return _RXbuffer[get_rx_buffer_read_position()];
        } else {
            return -1;
        }
    }

    void Telnet_Server::advance_rx_buffer_write_position(size_t data_size) {
        if (get_rx_buffer_writable_size() < data_size) {
            grbl_send(CLIENT_ALL, "QQ/Telnet: tried to overflow write buffer\r\n");
            return;
        }
        _RXbufferSize += data_size;
    }

    int Telnet_Server::get_rx_buffer_writable_size() {
        return TELNETRXBUFFERSIZE - get_rx_buffer_write_position();
    }

    int Telnet_Server::get_rx_buffer_write_position() {
        return _RXbufferpos + _RXbufferSize;
    }

    void Telnet_Server::advance_rx_buffer_read_position(size_t data_size) {
        _RXbufferpos += data_size;
        _RXbufferSize -= data_size;
        if (get_rx_buffer_readable_size() == 0) {
            // We consumed all available data.
            // Reset the buffer so we can refill from the start.
            reset_rx_buffer_read_position();
            grbl_send(CLIENT_ALL, "QQ/Telnet: emptied read buffer\r\n");
        }
    }

    int Telnet_Server::get_rx_buffer_readable_size() {
        return _RXbufferSize;
    }

    int Telnet_Server::get_rx_buffer_read_position() {
        return _RXbufferpos;
    }

    void Telnet_Server::reset_rx_buffer_read_position() {
        _RXbufferpos = 0;
        _RXbufferSize = 0;
    }

    int Telnet_Server::available() {
        return get_rx_buffer_readable_size();
    }

    int Telnet_Server::get_rx_buffer_available() {
        return get_rx_buffer_writable_size();
    }

    bool Telnet_Server::push(uint8_t data) {
        log_i("[TELNET]push %c", data);
        if (get_rx_buffer_writable_size() >= 1) {
            _RXbuffer[get_rx_buffer_write_position()] = data;
            advance_rx_buffer_write_position(1);
            log_i("[TELNET]buffer size %d", _RXbufferSize);
            return true;
        }
        return false;
    }

    bool Telnet_Server::push(const uint8_t* data, int data_size) {
        if (get_rx_buffer_writable_size() >= data_size) {
            for (int i = 0; i < data_size; i++) {
                if (!push(data[i])) {
                    // This should be impossible, since we checked there was enough capacity in advance.
                    grbl_send(CLIENT_ALL, "QQ/Telnet/push: failed\r\n");
                }
                COMMANDS::wait(0);
                //vTaskDelay(1 / portTICK_RATE_MS);  // Yield to other tasks
            }
            return true;
        }
        return false;
    }

    int Telnet_Server::read(void) {
        int v = peek();
        if (v == -1) {
          grbl_send(CLIENT_ALL, ".");
          return v;
        }
        advance_rx_buffer_read_position(1);
        grbl_sendf(CLIENT_ALL, "[%c]", v);
        //log_d("[TELNET]read %c",char(v));
        return v;
    }

    Telnet_Server::~Telnet_Server() {
        end();
    }
}
#endif  // Enable TELNET && ENABLE_WIFI
