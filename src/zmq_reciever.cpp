#include "zmq_reciever.hpp"

/* Generic includes */
#include <iostream>
#include <pthread.h>

/* ZMQ includes */
#include <zmq.hpp>


UBX_MODULE_LICENSE_SPDX(BSD-3-Clause)


void* recieverThread(void* arg);

/* define a structure for holding the block local state. By assigning an
 * instance of this struct to the block private_data pointer (see init), this
 * information becomes accessible within the hook functions.
 */
struct zmq_reciever_info
{
        /* add custom block local data here */

		// ZMQ context handle
		zmq::context_t* context;

		// ZMQ publisher
		zmq::socket_t* subscriber;

		// Thread that does the work
	    pthread_t workerThread;

        /* this is to have fast access to ports for reading and writing, without
         * needing a hash table lookup */
        struct zmq_reciever_port_cache ports;
};

/* init */
int zmq_reciever_init(ubx_block_t *b)
{
        int ret = -1;
        struct zmq_reciever_info *inf;
        unsigned int tmplen;
        char *connection_spec_str;
        std::string connection_spec;

        /* allocate memory for the block local state */
        if ((inf = (struct zmq_reciever_info*)calloc(1, sizeof(struct zmq_reciever_info)))==NULL) {
                ERR("zmq_reciever: failed to alloc memory");
                ret=EOUTOFMEM;
                goto out;
        }
        b->private_data=inf;
        update_port_cache(b, &inf->ports);

        try {

        	inf->context = new zmq::context_t(1);
        	inf->subscriber = new zmq::socket_t(*inf->context, ZMQ_SUB);

        	connection_spec_str = (char*) ubx_config_get_data_ptr(b, "connection_spec", &tmplen);
			connection_spec = std::string(connection_spec_str);
			std::cout << "ZMQ connection configuration for block " << b->name << " is " << connection_spec << std::endl;

        	inf->subscriber->connect(connection_spec_str);
        	inf->subscriber->setsockopt(ZMQ_SUBSCRIBE, "1", 0); // message filter options

		} catch (std::exception e) {
			std::cout << e.what() << " : " << zmq_strerror (errno) << std::endl;

			goto out;
		}


        ret=0;
out:
        return ret;
}

/* start */
int zmq_reciever_start(ubx_block_t *b)
{
        struct zmq_reciever_info *inf = (struct zmq_reciever_info*) b->private_data;

		/* The worker thread handles all incomming data */
		pthread_create(&inf->workerThread, NULL, recieverThread, b);

        int ret = 0;
        return ret;
}

/* stop */
void zmq_reciever_stop(ubx_block_t *b)
{
        /* struct zmq_reciever_info *inf = (struct zmq_reciever_info*) b->private_data; */
}

/* cleanup */
void zmq_reciever_cleanup(ubx_block_t *b)
{
		struct zmq_reciever_info *inf = (struct zmq_reciever_info*) b->private_data;
		delete inf->subscriber;
        free(b->private_data);
}

/* step */
void zmq_reciever_step(ubx_block_t *b)
{

        //struct zmq_reciever_info *inf = (struct zmq_reciever_info*) b->private_data;

}

void* recieverThread(void* arg) {

    ubx_block_t *b = (ubx_block_t *) arg;
    struct zmq_reciever_info *inf = (struct zmq_reciever_info*) b->private_data;
    std::cout << " recieverThread started." << std::endl;

    /* Receiver loop */
    while(true) {

    	zmq::message_t update;
    	inf->subscriber->recv(&update);
    	std::cout << "zmq_reciever: Recieved " << update.size() << " bytes from ?" << std::endl;

    	if (update.size() < 1) {
    		std::cout << "recv()" << std::endl;
    		break;
    	}


    	// move to step function?
    	ubx_type_t* type =  ubx_type_get(b->ni, "unsigned char");
		ubx_data_t msg;
		msg.data = (void *)update.data();
		msg.len = update.size();
		msg.type = type;

		//hexdump((unsigned char *)msg.data, msg.len, 16);
		__port_write(inf->ports.zmq_in, &msg);

		/* Inform potential observers ? */
    }

    /* Clean up */
    return 0;

}
