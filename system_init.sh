#!/bin/bash

#install gcc if not install
if_install_gcc=`gcc 2>&1`
if [[ $if_install_gcc == *'command not found'* ]];then
	yum install -y gcc	
fi

#install g++ if not install
if_install_gplusplus=`g++ 2>&1`
if [[ $if_install_gplusplus == *'command not found'* ]];then
	yum install -y gcc-c++
fi

#install gdb if not install
if_install_gdb=`gdb 2>&1`
if [[ $if_install_gdb == *'command not found'* ]];then
	yum install -y gdb
fi


#install ftp if not install
if_install_ftp=`rpm -qa | grep vsftpd`
if [[ "$if_install_ftp" == "" ]];then
	yum -y install vsftpd
	chkconfig vsftpd on
fi
