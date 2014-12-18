Linux
=====

These samples have been tested with Proton-C versions 0.4 through 0.8.

1) For version 0.4, use Makefile.0.4. Other versions use Makefile.
2) At the top of the appropriate makefile:
  a) Change PROTONVER to the version of Proton-C that you're using.
  b) Change PROTONROOT to point to the location of your build of Proton-C.
  c) Change CFLAGS to define SERVICEBUS_DOMAIN if your namespace does not
     use the standard domain servicebus.windows.net (for example, the China
     regions).
3) Run "make all" or "make -f Makefile.0.4 all" as appropriate.


Windows
=======

These samples have been tested with Proton-C version 0.6 built with OpenSSL.

1) At the top of Makefile.win32:
  a) Change PROTONVER to the version of Proton-C that you're using.
  b) Change PROTONROOT to point to the location of your build of Proton-C.
  c) Change PROTONCONFIG to be the configuration you used to build Proton-C.
  d) Change CFLAGS to define SERVICEBUS_DOMAIN if your namespace does not
     use the standard domain servicebus.windows.net (for example, the China
     regions).
2) In a Visual Studio x86 command prompt, run "nmake /f Makefile.win32 all".


Running
=======

Before running the samples, it is necessary to create the Service Bus queue or
topic+subscription that you will run the samples against. This can be done through
the portal or in any other way available. NOTE: these samples do not support
partitioned entities, which are now the default when creating through the portal,
so if creating through the portal, be sure to uncheck that box.

To run the sender:

    sender ServiceBusNamespace EntityPath IssuerName Key

where

    ServiceBusNamespace is the name of your Service Bus namespace. The full DNS
      name is composed by appending the SERVICEBUS_DOMAIN set during compilation.
      Usually the result is of the form yournamespace.servicebus.windows.net.

    EntityPath is the path to the entity (queue or topic) that you wish to
      send messages to. In the simplest case it would be "yourqueue" or "yourtopic",
      but Service Bus also supports paths so it could be "something/yourentity".

    IssuerName depends on whether you are using ACS or SAS. For ACS it will
      usually be "owner". For SAS, it will be the name of the SAS rule you created,
      or the default "RootManageSharedAccessKey".

    Key is the base64-encoded key associated with the IssuerName.

The sender always sends a fixed set of four messages, which demonstrate different
formats for the body contents.

The receiver uses the same command-line arguments as the sender, with one important
difference: to receive from a subscription, the EntityPath will be of the
form topicpath/Subscriptions/subscriptionname.

The receiver receives messages from the specified entity in a loop until the receive
call times out, and prints the header and contents of each received message. It
receives in PeekLock mode, since that is the most common customer scenario, and the
code shows up to set up that mode for each version of Proton-C.
