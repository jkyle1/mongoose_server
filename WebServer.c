//Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
#include <stdio.h>
#include <stdlib.h>
#include "mongoose.h"

#define TRUE 1
#define PORT "8080"

static struct mg_serve_http_opts opts;
static void send_response(struct mg_connection *c, double what) {
    mg_printf(c, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(c, "{ \"result\": %lf }", what);
    mg_send_http_chunk(c, "", 0);
}

static double get_x(struct http_message *req) {
    char s[32];
    mg_get_http_var(&req->body, "x", s, sizeof(s));
    return strtod(s, NULL);
}

static void printf_str(const struct mg_str *s) {
    const char *p = s -> p;
    int n = s -> len;
    for (int k = 0; k < n; k++) printf("%c", p[k]);
    printf("\n");
}

static void e_handler(struct mg_connection *c, int e, void *mess) {
    static int dir_listing = 0;
    char *yes_no[] = {"yes", "no"};
    struct http_message *req = (struct http_message *)mess;
    if (e == MG_EV_HTTP_REQUEST) {
        printf_str(&req->message);
        if(mg_vcmp(&req->uri, "/toggle") == 0) {
            dir_listing = 1 - dir_listing;
            opts.enable_directory_listing = yes_no[dir_listing];
            mg_printf(c, "%s", "HTTP/1.1 200 OK\r\n\r\n");
        }
        else if (mg_vcmp(&req->uri, "/times/two") == 0) {
            double x = get_x(req);
            send_response(c, x*2);
        }
        else if (mg_vcmp(&req->uri, "/times/three") == 0) {
            double x = get_x(req);
            send_response(c, x*3);
        }
        else {
            mg_serve_http(c, req, opts);
        }
    }
    else {
        if (e > 0) printf("Received event %d\n", e);
    }
}

int main(int argc, char *argv[]) {
    //initialise mongoose event manager
    struct mg_mgr server;
    mg_mgr_init(&server, NULL);

    //connect server to port and set callback function to handle incoming events
    struct mg_connection *conn = mg_bind(&server, PORT, e_handler);
    if (conn == NULL) {
        fprintf(stderr, "Error starting server on port %s\n", PORT);
        exit(EXIT_FAILURE);
    }
    //set server protocol
    mg_set_protocol_http_websocket(conn);

    //set document root to subdirectory ROOT
    opts.document_root = "/home/kyle/Dev/C++/mongoose_server/cmake-build-debug";
    {
        char *p = strrchr(argv[0], '/');
        if (p != NULL) {
            char *pp = strstr(argv[0], "Release");
            if (pp == NULL) pp = strstr(argv[0], "Debug");
            p = (pp != NULL) ? pp : p + 1;
            sprintf(p, "ROOT");
        }
        opts.document_root = argv[0];
        }

    //start server
    printf("Starting server on port %s\n", PORT);
    while (TRUE) {
        mg_mgr_poll(&server, 1000);
    }

    //free up memory
    mg_mgr_free(&server);
    return EXIT_SUCCESS;
}




















