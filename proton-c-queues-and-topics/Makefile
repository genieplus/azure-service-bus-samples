##
##  Copyright 2014 Microsoft Corporation
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##    http://www.apache.org/licenses/LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.
##

##
## PROTONVER is the minor version, since all versions of Proton are 0.x
## PROTONROOT is where you untarred Proton, which will usually be
##     something/qpid-proton-0.x
## PROTONLIBVER is 2 for 0.5 and up, 1 for 0.4
##
PROTONVER := 8
PROTONROOT := ~/proton/qpid-proton-0.$(PROTONVER)
PROTONLIBVER := 2

CFLAGS := -g -DSERVICEBUS_DOMAIN="\"servicebus.windows.net\""
LDFLAGS := -luuid

OPTS := -I $(PROTONROOT)/proton-c/include \
	-I $(PROTONROOT)/build/proton-c/include
LIBS := $(PROTONROOT)/build/proton-c/libqpid-proton.so

# Version 0.4 requires slightly different options
#OPTS := -I $(PROTONROOT)/proton-c/include -DPN_VERSION_MAJOR=0 \
	-DPN_VERSION_MINOR=4
#LIBS := $(PROTONROOT)/build/proton-c/libqpid-proton.so

OBJDIR := objs
BINDIR := bins


all:	$(OBJDIR) $(BINDIR) $(BINDIR)/0$(PROTONVER) \
	$(BINDIR)/0$(PROTONVER)/sender0$(PROTONVER) \
	$(BINDIR)/0$(PROTONVER)/receiver0$(PROTONVER) \
	$(BINDIR)/0$(PROTONVER)/libqpid-proton.so.$(PROTONLIBVER)

$(OBJDIR):
	mkdir $@

$(BINDIR):
	mkdir $@

$(BINDIR)/0$(PROTONVER):
	mkdir $@


$(BINDIR)/0$(PROTONVER)/sender0$(PROTONVER):	\
	$(OBJDIR)/sender0$(PROTONVER).o $(OBJDIR)/common0$(PROTONVER).o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

$(OBJDIR)/sender0$(PROTONVER).o:	sender.c common.h
	$(CC) $(CFLAGS) -c $(OPTS) -o $@ $<

$(BINDIR)/0$(PROTONVER)/receiver0$(PROTONVER):	\
	$(OBJDIR)/receiver0$(PROTONVER).o $(OBJDIR)/common0$(PROTONVER).o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

$(OBJDIR)/receiver0$(PROTONVER).o:	receiver.c common.h
	$(CC) $(CFLAGS) -c $(OPTS) -o $@ $<

$(OBJDIR)/common0$(PROTONVER).o:	common.c common.h
	$(CC) $(CFLAGS) -c $(OPTS) -o $@ $<

$(BINDIR)/0$(PROTONVER)/libqpid-proton.so.$(PROTONLIBVER):	\
	$(LIBS).$(PROTONLIBVER)
	cp $< $(BINDIR)/0$(PROTONVER)
