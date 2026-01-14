#include <flecs.h>
#include <globals.h>
#include <login_screen.h>
#include <message_processor.h>
#include <raygui.h>
#include <tcp.h>

void LoginScreen(ecs_world_t* ctx)
{
    static bool edit_name = false;
    static bool edit_host = false;
    static bool edit_port = false;
    static bool tried_connecting = false;

    int widget_padding = 10;

    Rectangle widget_pos = {
        .x = widget_padding,
        .y = widget_padding,
        .height = 40
    };

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    widget_pos.width = 500;

    if (GuiTextBox(widget_pos, globals.name, sizeof(globals.name), edit_name))
        edit_name = !edit_name;
    widget_pos.y += widget_pos.height + widget_padding;

    if (GuiTextBox(widget_pos, globals.host, sizeof(globals.host), edit_host))
        edit_host = !edit_host;
    widget_pos.y += widget_pos.height + widget_padding;

    if (GuiTextBox(widget_pos, globals.port, sizeof(globals.port), edit_port))
        edit_port = !edit_port;
    widget_pos.y += widget_pos.height + widget_padding;

    widget_pos.width = 200;

    if (GuiButton(widget_pos, "Connect") && globals.server_sock == SOCK_INVALID) {
        tried_connecting = true;
        globals.server_sock = tcp_connect(globals.host, globals.port);

        if (globals.server_sock != SOCK_INVALID) {
            TraceLog(LOG_INFO, "Sending Hello message");

            send_message(&globals.send_buf,
                ((struct message) {
                    .type = MESSAGE_HELLO,
                }),
                .from_id = globals.name);
        }
    }
    widget_pos.y += widget_pos.height + widget_padding;

    if (tried_connecting) {
        if (globals.server_sock == SOCK_INVALID)
            GuiLabel(widget_pos, "Failed to connect");
        else if (globals.server_sock != SOCK_INVALID)
            GuiLabel(widget_pos, "Connecting...");
    }

    widget_pos.y += widget_pos.height + widget_padding;

    MessageProcessor(ctx);
}
