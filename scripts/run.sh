# Copyright(C) 2011-2017 Pedro H. Penna <pedrohenriquepenna@gmail.com>
# 
# This file is part of Nanvix.
# 
# Nanvix is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# Nanvix is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
#

export BINDIR=$PWD/bin

trap 'kill $(jobs -p)' EXIT

bin/ramdisk /dev/ramdisk0 2> /dev/null & sleep 1
bin/ramdisk /dev/ramdisk1 2> /dev/null & sleep 1
bin/ramdisk /dev/ramdisk2 2> /dev/null & sleep 1
bin/ramdisk /dev/ramdisk3 2> /dev/null & sleep 1
bin/bdev    /sys/bdev     2> /dev/null & sleep 1

for i in {1..30}; do
	bin/memwrite.benchmark 2 2>&1 | grep memwrite
done
