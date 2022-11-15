#!/bin/bash


do_build()
{
	rm -rf build >> /dev/null
	mkdir build
	pushd build
	cmake .. 
	make
	popd
}


do_build

