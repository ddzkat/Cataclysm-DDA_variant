
#ifndef MULTIPLAY_MANAGER_H_
#define MULTIPLAY_MANAGER_H_

#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>

#include "type_id.h"
#include "itype.h"
#include "output.h"
#include "debug.h"
#include "options.h"

class multiplay_client {


    public:
        multiplay_client( );
        // ~multiplay_client() = delete;

    private:
        std::thread client_thread;
        SOCKET sock;
        char recv_buffer[8];
        bool is_working_value = false;
    public:
        void start_client_thread( SOCKET );
        void client_work_finish_cleanup();
        void client_thread_process();
        bool is_working();

};

enum client_mode {
    mode_default = 0,
    mode_quit,
    mode_message,
    mode_spawn,
    mode_despawn,
    mode_dash,
    mode_auto,
};

enum command_type {
    client_command_nop = 0,
    client_command_message,
    client_command_spawn,
    client_command_despawn,
    client_command_move,
    client_command_move_repeatly,
    client_command_special_attack,
    client_command_auto_move,
};

class client_command {
    public:
        std::string client_name;
        command_type c_type;
        std::string command_argument;
        client_command( ){};

        client_command( std::string new_client_id, command_type new_c_type, std::string new_command_argument ) {
            client_name = new_client_id;
            c_type = new_c_type;
            command_argument = new_command_argument;
        }
};

class multiplay_manager {

    private:
        std::thread server_thread;
        // std::vector<multiplay_client> client_list;

        bool is_server_active = false;
    public:
        int start_server();
        int stop_server();

        int multiplay_menu();

        void server_thread_process();

        int do_turn();

        void insert_command(std::string, client_command);
        client_command find_command(std::string);
        void erase_command(std::string);

};


#endif /* MULTIPLAY_MANAGER_H_ */
