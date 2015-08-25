#!/usr/bin/perl
#udpclient.pl

use IO::Socket::INET;

# flush after every write
$| = 1;

my ($socket,$data);

#  We call IO::Socket::INET->new() to create the UDP Socket
# and bind with the PeerAddr.
$socket = new IO::Socket::INET (
PeerAddr   => '192.168.0.6:5000',
Proto        => 'udp'
) or die "ERROR in Socket Creation : $!\n";
#send operation
$data = "data from client";
$socket->send($data);

#read operation
$data = <$socket>;
print "Data received from socket : $data\n ";

sleep(10);
$socket->close();
