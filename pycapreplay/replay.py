
import pcap
import dpkt
import sys
import socket

def usage():
    print "python replay.py <pcap>"
    sys.exit(1)

def replay(pkts, fromip, toip):

    print "import socket"
    print "s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)"
    print "s.connect(('%s', %d))" % ('127.0.0.1', 23)
    for t, p in pkts:
        eth = dpkt.ethernet.Ethernet(p)
        ip = eth.ip
        tcp = ip.tcp
        if ip.src == fromip and ip.dst == toip and len(ip.tcp.data) > 0:
            print "s.send(b'%s')" % repr(ip.tcp.data)
        elif ip.src == toip and ip.dst == fromip and len(ip.tcp.data) > 0:
            print "s.recv(%u)" % len(ip.tcp.data)
    print "s.close()"

def main(argv):
    if len(argv) != 3:
        usage()

    replay(pcap.pcap(argv[0]), socket.inet_aton(argv[1]), socket.inet_aton(argv[2]))

if __name__ == '__main__':
    main(sys.argv[1:])

