#include "zmq_reciever.hpp"

/* edit and uncomment this:
 * UBX_MODULE_LICENSE_SPDX(GPL-2.0+)
 */

/* define a structure for holding the block local state. By assigning an
 * instance of this struct to the block private_data pointer (see init), this
 * information becomes accessible within the hook functions.
 */
struct zmq_reciever_info
{
        /* add custom block local data here */

        /* this is to have fast access to ports for reading and writing, without
         * needing a hash table lookup */
        struct zmq_reciever_port_cache ports;
};

/* init */
int zmq_reciever_init(ubx_block_t *b)
{
        int ret = -1;
        struct zmq_reciever_info *inf;

        /* allocate memory for the block local state */
        if ((inf = (struct zmq_reciever_info*)calloc(1, sizeof(struct zmq_reciever_info)))==NULL) {
                ERR("zmq_reciever: failed to alloc memory");
                ret=EOUTOFMEM;
                goto out;
        }
        b->private_data=inf;
        update_port_cache(b, &inf->ports);
        ret=0;
out:
        return ret;
}

/* start */
int zmq_reciever_start(ubx_block_t *b)
{
        /* struct zmq_reciever_info *inf = (struct zmq_reciever_info*) b->private_data; */
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
        free(b->private_data);
}

/* step */
void zmq_reciever_step(ubx_block_t *b)
{
        /*
        struct zmq_reciever_info *inf = (struct zmq_reciever_info*) b->private_data;
        */
}

