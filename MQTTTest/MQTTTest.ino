#include <ESP8266WiFi.h>                  // Include the WiFi library.
#include <MQTT.h>                         // Include the MQTT library.
#include <Wire.h>                         // Include Wire library, required for I2C devices
#include <Arduino_JSON.h>                 // Include the JSON library.

const char* ssid     = "Red";             // The case-sensitive SSID of the WiFi network you want to connect to.
const char* password = "8012254722";      // The password of the WiFi network.
const char* broker = "192.168.55.200";         // The MQTT broker name.
const String mqttTopic = "MQTTTest001";    // The MQTT topic.
WiFiClient wifiClient;
MQTTClient mqttClient( 512 );             // Create a client with a 512 byte buffer.  The default is 128 bytes.
unsigned long lastMillis = 0;
unsigned long loopCount = 0;


void connect()
{
  Serial.print( "Checking wifi..." );
  while ( WiFi.status() != WL_CONNECTED )
  {
    Serial.print( "." );
    delay( 1000 );
  }

  Serial.print( "\nMQTT connecting..." );
  while ( !mqttClient.connect( "arduino" ) )
  {
    Serial.print( "." );
    delay( 1000 );
  }

  Serial.println( "\nMQTT connected!" );

//  mqttClient.subscribe( "/hello" );
  // mqttClient.unsubscribe( "/hello" );
} // End of connect() function.


void messageReceived( String &topic, String &payload )
{
  Serial.println( "incoming: " + topic + " - " + payload );

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
} // End of messageReceived() function.


/**
 * Build a JSON object for the CreatePersistenceTopic operation.
 * This operation will create a MQTT topic that can be published to.
 * This operation will also create a table in cTree Edge.
 * Consecutive calls to this function will not alter the topic or table.
 */
void createTopic( String topic )
{
  JSONVar createObject;
  createObject["operation"] = "CreatePersistenceTopic";
  createObject["persistenceTopic"] = topic;
  createObject["tableName"] = topic;          // It is customary, but not required, for the table name to match the topic.
  createObject["mapOfPropertiesToFields"]["propertyPath"] = "BIGINT001";
  createObject["mapOfPropertiesToFields"]["fieldName"] = "BIGINT001";
  createObject["mapOfPropertiesToFields"]["fieldType"] = "BIGINT";
  
  mqttClient.publish( "ctreeAdministration", createObject );
} // End of createTopic() function.


/**
 * Build a JSON object for the DeletePersistenceTopic operation.
 * This operation will delete an existing MQTT topic.
 * This operation does not delete the table in cTree Edge.
 * Consecutive calls to this function will have no effect aside from logging an error in Edge.
 */
void deleteTopic( String topic )
{
  JSONVar deleteObject;
  deleteObject["operation"] = "DeletePersistenceTopic";
  deleteObject["persistenceTopic"] = topic;
  
  mqttClient.publish( "ctreeAdministration", deleteObject );
} // End of deleteTopic() function.


void setup()
{
  Serial.begin( 115200 );                 // Start the Serial communication to send messages to the computer.
  delay( 10 );
  Serial.println( '\n' );

  WiFi.begin( ssid, password );           // Connect to the network
  Serial.print( "WiFi connecting to " );
  Serial.println( ssid );

  int i = 0;
  /*
     WiFi.status() return values:
     0 : WL_IDLE_STATUS when WiFi is in process of changing between statuses
     1 : WL_NO_SSID_AVAIL in case configured SSID cannot be reached
     3 : WL_CONNECTED after successful connection is established
     4 : WL_CONNECT_FAILED if password is incorrect
     6 : WL_DISCONNECTED if module is not configured in station mode
  */
  while ( WiFi.status() != WL_CONNECTED ) // Wait for the WiFi to connect.
  {
    delay( 1000 );
    Serial.println( "Waiting for a connection..." );
    Serial.print( "WiFi status: " );
    Serial.println( WiFi.status() );
    Serial.print( ++i );
    Serial.println( " seconds" );
  }

  Serial.println( '\n' );
  Serial.println( "Connection established!" );
  Serial.print( "IP address:\t" );
  Serial.println( WiFi.localIP() );    // Send the IP address of the ESP8266 to the computer.

  pinMode( LED_BUILTIN, OUTPUT );      // Initialize digital pin LED_BUILTIN as an output.

  mqttClient.begin( "127.0.0.1", wifiClient );
  mqttClient.onMessage( messageReceived );

  connect();

   createTopic( mqttTopic );
} // End of setup() function.


void loop()
{
  mqttClient.loop();

  if ( !mqttClient.connected() )
  {
    connect();
  }

  // Publish a message roughly every second.
  if ( millis() - lastMillis > 1000 )
  {
    lastMillis = millis();
    // bool publish(const String &topic, const String &payload);
//    mqttClient.publish( "/hello", "world" );
    char * myStr;
    sprintf( myStr, "%lu", loopCount );
    JSONVar publishObject;
    publishObject[mqttTopic] = myStr;
    mqttClient.publish( "MQTTTest001", publishObject );
  }

  digitalWrite( LED_BUILTIN, HIGH );   // Turn the LED on.
  delay( 1000 );                       // Wait for one second.
  digitalWrite( LED_BUILTIN, LOW );    // Turn the LED off.
  delay( 1000 );                       // Wait for one second.
  loopCount++;
  Serial.print( "Loop count: " );
  Serial.println( loopCount );
  Serial.print( "IP address:\t" );
  Serial.println( WiFi.localIP() );    // Send the IP address of the ESP8266 to the serial port.
} // End of loop() function.
