/*
 *  Copyright 2014 Microsoft Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
*/

import org.apache.qpid.amqp_1_0.jms.impl.MessageImpl;

import javax.jms.*;
import javax.naming.Context;
import javax.naming.InitialContext;
import java.io.*;
import java.net.URLEncoder;
import java.util.Hashtable;

public class SimpleEventHubSender {
    private Connection connection;
    private Session sendSession;
    private MessageProducer sender;

    private final String domain = "servicebus.windows.net";

    public SimpleEventHubSender(String namespace, String eventhubName, String issuer, String key) throws Exception
    {
        //
        // Create the servicebus.properties file
        //
        String encodedKey = URLEncoder.encode(key, "UTF-8");
        String connectionString = "amqps://" + issuer + ":" + encodedKey + "@" + namespace + "." + domain;
        String propertiesFileName = "servicebus.properties";
        File propertiesFile = new File(propertiesFileName);
        if (propertiesFile.exists())
        {
            propertiesFile.delete();
            propertiesFile.createNewFile();
        }
        BufferedWriter writer = new BufferedWriter(new FileWriter(propertiesFile));
        writer.write("connectionfactory.SBCF = " + connectionString);
        writer.newLine();
        writer.write("queue.EVENTHUB = " + eventhubName);
        writer.newLine();
        writer.close();

        //
        // Configure JNDI environment
        //
        Hashtable<String, String> env = new Hashtable<String, String>();
        env.put(Context.INITIAL_CONTEXT_FACTORY,
                "org.apache.qpid.amqp_1_0.jms.jndi.PropertiesFileInitialContextFactory");
        env.put(Context.PROVIDER_URL, propertiesFileName);
        Context context = new InitialContext(env);

        //
        // Lookup ConnectionFactory and Queue
        //
        ConnectionFactory cf = (ConnectionFactory) context.lookup("SBCF");
        Destination eventhub = (Destination) context.lookup("EVENTHUB");

        //
        // Create Connection, Session, and MessageProducer.
        //
        connection = cf.createConnection();
        sendSession = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        sender = sendSession.createProducer(eventhub);
    }

    public static void main(String[] args)
    {
        if (args.length != 4)
        {
            System.out.println("Arguments: namespace path issuer-name issuer-key");
            System.out.println("There are three ways to send to an event hub:");
            System.out.println("  1) Allow the event hub to decide where to put the message:");
            System.out.println("     path argument = your event hub name");
            System.out.println("  2) Send all messages to a specific partition of the event hub:");
            System.out.println("     path argument = eventhubname/Partitions/partitionid");
            System.out.println("     for example: myhub/Partitions/3");
            System.out.println("  3) Provide a partition key which the event hub hashes to decide where to");
            System.out.println("     put the message (all messages with same key will go to same partition):");
            System.out.println("     path argument = your event hub name");
            System.out.println("     provide partition keys at runtime");
            return;
        }

        try
        {
            SimpleEventHubSender simpleSender = new SimpleEventHubSender(args[0], args[1], args[2], args[3]);

            BufferedReader commandLine = new java.io.BufferedReader(new InputStreamReader(System.in));

            int messageNumber = 0;
            while (true)
            {
                System.out.println("Press [enter] to send a msg without partition key.");
                System.out.println("Type a string and press [enter] to send a msg with partition key.");
                System.out.println("Type 'exit' and press [enter] to quit.");
                System.out.print("Ready> ");
                String s = commandLine.readLine();
                if (s.equalsIgnoreCase("exit"))
                {
                    simpleSender.close();
                    break;
                }
                else
                {
                    simpleSender.sendMessage(messageNumber, s);
                }
                messageNumber++;
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    private void sendMessage(int messageNumber, String partitionKey) throws JMSException
    {
        // Send a TextMessage or a BytesMessage.

        //TextMessage message = sendSession.createTextMessage();
        //message.setText("Test AMQP message from JMS");

        BytesMessage message = sendSession.createBytesMessage();
        byte[] body = { 0x1, 0x2, 0x3, 0x4, 0x5 };
        message.writeBytes(body);

        message.setJMSMessageID("ID:sample" + messageNumber);
        message.setStringProperty("SampleProperty", "SampleValue");

        System.out.print("Sending message with JMSMessageID = " + message.getJMSMessageID());
        if (partitionKey.length() > 0)
        {
            // THIS CODE REQUIRES A PRERELEASE VERSION OF THE QPID JMS AMQP 1.0 CLIENT
            // The ability to set message annotations is present in the trunk and should be
            // released in the next version (0.32) but does not exist in 0.30. To use this
            // code with the Qpid JMS AMQP 1.0 client version 0.30 or earlier, comment out
            // all the lines in this block.
            System.out.print(" and partition key = " + partitionKey);
            String jsonified = "{ \"x-opt-partition-key\" : \"" + partitionKey + "\" }";
            message.setStringProperty(MessageImpl.JMS_AMQP_MESSAGE_ANNOTATIONS, jsonified);
        }
        System.out.println();

        sender.send(message);
        System.out.println("Sent message OK");
    }

    public void close() throws JMSException
    {
        connection.close();
    }
}