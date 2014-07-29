#include "zmq_sender.hpp"

/* Generic includes */
#include <iostream>

/* ZMQ includes */
#include <zmq.hpp>

UBX_MODULE_LICENSE_SPDX(BSD-3-Clause)

/* define a structure for holding the block local state. By assigning an
 * instance of this struct to the block private_data pointer (see init), this
 * information becomes accessible within the hook functions.
 */
struct zmq_sender_info
{
        /* add custom block local data here */

        // ZMQ context handle
        zmq::context_t* context;

        // ZMQ publisher
        zmq::socket_t* publisher;

        /* this is to have fast access to ports for reading and writing, without
         * needing a hash table lookup */
        struct zmq_sender_port_cache ports;
};

/* init */
int zmq_sender_init(ubx_block_t *b)
{
        int ret = -1;
        struct zmq_sender_info *inf;
        unsigned int tmplen;
        char *connection_spec_str;
        std::string connection_spec;


        /* allocate memory for the block local state */
        if ((inf = (struct zmq_sender_info*)calloc(1, sizeof(struct zmq_sender_info)))==NULL) {
                ERR("zmq_sender: failed to alloc memory");
                ret=EOUTOFMEM;
                goto out;
        }
        b->private_data=inf;
        update_port_cache(b, &inf->ports);

        inf->context = new zmq::context_t(1);
		inf->publisher = new zmq::socket_t(*inf->context, ZMQ_PUB);
		connection_spec_str = (char*) ubx_config_get_data_ptr(b, "connection_spec", &tmplen);
		inf->publisher->bind(connection_spec_str);

		connection_spec = std::string(connection_spec_str);
		std::cout << "ZMQ connection configuration for block " << b->name << " is " << connection_spec << std::endl;

        ret=0;
out:
        return ret;
}

/* start */
int zmq_sender_start(ubx_block_t *b)
{
        /* struct zmq_sender_info *inf = (struct zmq_sender_info*) b->private_data; */
        int ret = 0;
        return ret;
}

/* stop */
void zmq_sender_stop(ubx_block_t *b)
{
        /* struct zmq_sender_info *inf = (struct zmq_sender_info*) b->private_data; */
}

/* cleanup */
void zmq_sender_cleanup(ubx_block_t *b)
{
        free(b->private_data);
}

/* step */
void zmq_sender_step(ubx_block_t *b)
{

        struct zmq_sender_info *inf = (struct zmq_sender_info*) b->private_data;
        std::cout << "zmq_sender: Processing an incoming update" << std::endl;

		/* Read data from port */
		ubx_port_t* port = inf->ports.zmq_out;
		assert(port != 0);

		ubx_data_t msg;
		checktype(port->block->ni, port->in_type, "unsigned char", port->name, 1);
		msg.type = port->in_type;
		msg.len = 1;
		__port_read(port, &msg);

		/* Setup ZMQ message*/
		zmq::message_t message(msg.len);
		memcpy(message.data(), (char *)msg.data, msg.len);

		/* Send the message */
		inf->publisher->send(message);

}

