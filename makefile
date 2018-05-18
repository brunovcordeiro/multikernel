# Copyright(C) 2011-2018 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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

all: async mailbox portal rmem

async:
	cd $(CURDIR)/src/test/async/; make;

mailbox:
	cd $(CURDIR)/src/test/mailbox/; make;

portal:
	cd $(CURDIR)/src/test/portal/; make;

rmem:
	cd $(CURDIR)/src/test/rmem/; make;

clean:
	cd $(CURDIR)/src/test/async/; make clean;
	cd $(CURDIR)/src/test/mailbox/; make clean;
	cd $(CURDIR)/src/test/portal/; make clean;
	cd $(CURDIR)/src/test/rmem/; make clean;
