#!/usr/bin/env python

import random
import os, sys, uuid
import socket, struct

protos = ["http://", "ftp://"]
suffixs = ["xml", "txt", "jpg"]

RANDOM_IP_POOL=['192.168.10.222/0']

loop = 101

def get_random_ip():
    str_ip = RANDOM_IP_POOL[random.randint(0,len(RANDOM_IP_POOL) - 1)]
    str_ip_addr = str_ip.split('/')[0]
    str_ip_mask = str_ip.split('/')[1]
    ip_addr = struct.unpack('>I',socket.inet_aton(str_ip_addr))[0]
    mask = 0x0
    for i in range(31, 31 - int(str_ip_mask), -1):
        mask = mask | ( 1 << i)
    ip_addr_min = ip_addr & (mask & 0xffffffff)
    ip_addr_max = ip_addr | (~mask & 0xffffffff)
    return socket.inet_ntoa(struct.pack('>I', random.randint(ip_addr_min, ip_addr_max)))

def get_file_name():
    file_str  = ''.join(random.sample(['z','y','x','w','v','u','t','s','r','q','p','o','n','m','l','k','j','i','h','g','f','e','d','c','b','a'], 10))
    tmp_file_name = 'tmpfile.%s' %  random.choice(suffixs) 
    return tmp_file_name

with open(sys.argv[1], 'wt') as f:
    for i in xrange(1, 2**16):
        proto = random.choice(protos)
	ip = get_random_ip()
        port = random.randint(10000, 20000)
	port_str = '%d' % port
	path1 = ''.join(random.sample(['z','y','x','w','v','u','t','s','r','q','p','o','n','m','l','k','j','i','h','g','f','e','d','c','b','a'], 5))
	path2 = ''.join(random.sample(['z','y','x','w','v','u','t','s','r','q','p','o','n','m','l','k','j','i','h','g','f','e','d','c','b','a'], 15))
	file_name = get_file_name()	
        ts_url = ''.join([proto, ip, ':', port_str, '/', path1, '/', path2, '/', file_name, '\n'])
	
	#loop = 101
	#loop = random.randint(1, 101)
	for i in range(loop):
		f.write(ts_url)
    	if loop > 1:
        	loop = loop - 1; 
