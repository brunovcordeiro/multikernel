#
# MIT License
#
# Copyright (c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.  THE SOFTWARE IS PROVIDED
# "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
# LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

source "scripts/arch/mppa256.sh"

echo "=== Running Core and NoC Interface Tests"
run2 "nanvix-debug.img" "/test-driver" "/servers1" "--debug --hal-core" | grep nanvix

echo "=== Running Unnamed Sync Tests"
run2 "nanvix-debug.img" "/test-driver" "/servers1" "--debug --hal-sync" | grep nanvix
run2 "nanvix-debug.img" "/test-driver" "/servers1" " "                  | grep nanvix

echo "=== Running Unnamed Mailbox Tests"
run2 "nanvix-debug.img" "/test-driver" "/servers1" "--debug --hal-mailbox" | grep nanvix

echo "=== Running Unnamed Portal Tests"
run2 "nanvix-debug.img" "/test-driver" "/servers1" "--debug --hal-portal" | grep nanvix

echo "=== Running Naming Service Tests"
run2 "nanvix-debug.img" "/test-driver" "/servers1" "--debug --name" | grep nanvix

echo "=== Running Nammed Mailbox Tests"
run2 "nanvix-debug.img" "/test-driver" "/servers1" "--debug --mailbox" | grep nanvix

echo "=== Running Barrier Tests"
run2 "nanvix-debug.img" "/test-driver" "/servers1" "--debug --barrier" | grep nanvix
