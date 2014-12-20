# Event Hub Sender sample for use with Qpid JMS AMQP 1.0 client

The Qpid JMS AMQP 1.0 client is available at http://qpid.apache.org/components/qpid-jms/index.html
Service Bus only supports AMQP 1.0, so you specifically need the AMQP 1.0 client.

## Things to consider:

1) The domain for the namespace is set to servicebus.windows.net. If your namespace is in a different domain (for example, you are in the China datacenters), you will need to change this in the code.

2) The ability to set a partition key on a message is dependent on a new feature in the Qpid JMS AMQP 1.0 client that is not present in the 0.30 release but should be when 0.32 is released. You can experiment with it now by getting the trunk code for the client and building your own JARs. Otherwise, you will need to comment out some lines before the code will compile -- see the comments in the sample.

## Running the sample

Before running the sample, it is necessary to create the Service Bus event hub that you will send messages to. This can be done through the portal or in any other way available.

Arguments:

    ServiceBusNamespace Path IssuerName Key

where

    ServiceBusNamespace is the name of your Service Bus namespace.

    Path depends on how you want the message to be assigned to a partition. See "Messages and Partitions" below.

    IssuerName depends on whether you are using ACS or SAS. For ACS it will
      usually be "owner". For SAS, it will be the name of the SAS rule you
      created, or the default "RootManageSharedAccessKey".

    Key is the base64-encoded key associated with the IssuerName.

When the sample is running, you will be prompted before each message is sent. You can send it without a partition key, or provide a partition key at the prompt, or type "exit" to quit.

## Messages and Partitions

If the path argument is just the name of the event hub, Service Bus checks for a partition key on the message. The partition key is an arbitrary string that Service Bus hashes down to a partition id. All messages with the same partition key are guaranteed to go to the same partition. If there is no partition key on the message, then Service Bus will round-robin the messages across the event hub's partitions.

The path argument can specify a partition, and all messages will be assigned to that partition. The syntax is eventhubname/Partitions/partitionid. For example, myhub/Partitions/3
