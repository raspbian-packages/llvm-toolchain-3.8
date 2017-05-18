#!/bin/bash -e
dpkg-architecture -qDEB_HOST_GNU_TYPE | sed -r s/arm/armv6/
