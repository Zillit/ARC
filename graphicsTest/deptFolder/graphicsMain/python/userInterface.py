import zmq

context =zmq.Context()

#From user to server
frontend = context.socket(zmq.SUB)
frontend.connect("tcp://nhkim91.ddns.net:2224")

#From server to user
frontreq = context.socket(zmq.REQ)
frontreq.connect("tcp://nhkim91.ddns.net:2225")

frontreq.send_string("t_STYROR_TestBithes")
# frontend.setsockopt_string(zmq.SUBSCRIBE, '10001'.decode('ascii'))

while True:
    try:
        message = frontend.recv_string(zmq.DONTWAIT)
        print message
    except zmq.Again:
        pass