/* Helper tool that is able to send a simple test message via nanomsg.
 *
 * ./sender <url_of_destination_socket>
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

int in_socket;
int out_socket;
union Data {
	int i;
	char* ch;
	float f;
	bool b;
};

union Data a_test_message[8];

char* test_message[] = {"1"};
void fatal(const char *func) {
        fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
}

char * date(void) {
        time_t now = time(&now);
        struct tm *info =localtime(&now);
        char *text = asctime(info);
        text[strlen(text)-1] = '\0'; // remove '\n'
        return (text);
}
void create_message(){
	union Data d1,d2,d3,d4,d5,d6,d7,d8;	

	d1.i = 47;
	d2.i = 1;
	d3.f = 2.22;
	d4.f = 3.33;
	d5.f = 4.44;
	d6.f = 5.55;
	d7.i = 1;	
	d8.i = 4;
	
	a_test_message[0] = d1;
	a_test_message[1] = d2;
	a_test_message[2] = d3;
	a_test_message[3] = d4;
	a_test_message[4] = d5;
	a_test_message[5] = d6;
	a_test_message[6] = d7;
	a_test_message[7] = d8;
}

/* 
        Function that sends a message onto out_socket
*/
int send(char* buf, int length) {
        int ret = 1;

        //char *message = date();
        //int length_message = strlen(test_message) + 1;
        //char *buf = &a_test_message;
	printf("SENDING: %s\n", buf);
        //FIXME: Remove hardcoded length of message...
        int bytes = nn_send(out_socket, buf, length, 0);

        if (bytes < 0) {
                fatal("nn_send");
                ret = 0;
        }
	else {
		printf("%i bytes sent\n", bytes);
	}

        return ret;
}

/*
        Function that realizes setup of communication

        url_in = adress of endpoint used for receiving
        url_out = adress of endpoint used for sending
*/
int setup(const char *url_out) {
        int ret = 1;

        ///////////////////////////////////////////////
        // Configure and bind out_socket
        ///////////////////////////////////////////////
        
        if ((out_socket = nn_socket(AF_SP, NN_PUB)) < 0) {
                fatal("nn_socket");
                ret = 0;
        }

        if (nn_bind(out_socket, url_out) < 0) {
                fatal("nn_bind");
                ret = 0;
        }
        
        return ret;

}

/*      Main-routine:
                1) Setup the communication sockets
                2) Send message over out_socket
                3) Wait 5 seconds
                4) Goto 2)
*/
int main(const int argc, const char **argv) {	
        // Option to set a filepath. If filepath is set != "", the message will be read from file.
        const char* filepath = "";
        char* message;
        size_t size;

        if ((argc >= 2)) {
                if (setup(argv[1]) != 1) {
                        printf("Error: Setup failed!\n");
                }
        }
        else {
                printf("Error: Not enough parameters!\n");
                printf("Usage: ./sender <url_of_destination_socket>\n");
                return -1;
        }
        
        if (filepath == "") {
                message = "thisIsATestMessage";
                //size = sizeof(message);
                size = 144;
        }
        else {
                FILE *pFile = fopen(filepath, "rb");
                
                // determine message size
                fseek(pFile, 0, SEEK_END);
                size = ftell(pFile)-1; // -1 to remove EOF identifier
                printf("size = %i", size);
                fseek(pFile, 0, SEEK_SET);
                
                if (pFile) {
                        message = malloc(size);
                        fread(message, size,1,pFile);
                        fprintf(stderr, "Successfully read from file...\n");
                }
                else {
                        fprintf(stderr, "Couldn't open file %s\n", filepath);
                }
                fclose(pFile);
        }

        for(int i = 0; i <= 5; ++i) {
                send(message, size);
                sleep(1);
        }        
        if(filepath != "")
                free(message);
}