return pkg
{
  name="ubx_zmq_bridge",
  path="../",
      
  dependencies = {
 --   { name="ros_bridge", type="cmake" },
  },
  
  
  blocks = {
    { name="zmq_sender", file="examples/zmq_sender.lua", src_dir="src" },
    { name="zmq_reciever", file="examples/zmq_reciever.lua", src_dir="src" },
  },
  
  libraries = {
    { name="zmqsenderlib", blocks={"zmq_sender"} },
    { name="zmqrecieverlib", blocks={"zmq_reciever"} },
  },  
}
