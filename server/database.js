/**
 * @file database.js
 * @description MongoDB database connection module for the WeatherStation application.
 * Manages database connection lifecycle and provides access to the SensorData collection.
 */

import { MongoClient, ServerApiVersion } from 'mongodb';

/**
 * @const {string} uri - MongoDB connection URI with authentication credentials
 * @private
 */
const uri = "mongodb+srv://Admin:Admin@cluster0.nx36qup.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";

/**
 * @const {MongoClient} client - MongoDB client instance configured with Server API v1
 * @private
 */
const client = new MongoClient(uri, {
  serverApi: {
    version: ServerApiVersion.v1,
    strict: true,
    deprecationErrors: true,
  }
});

/**
 * @private
 * @type {import('mongodb').Collection|undefined}
 * @description Reference to the SensorData collection in the WeatherStation database
 */
let collection;

/**
 * @async
 * @function connectDB
 * @description Establishes connection to MongoDB and initializes the SensorData collection.
 * Should be called before any database operations are performed.
 * @returns {Promise<void>}
 * @throws {Error} If connection to MongoDB fails
 * @example
 * await connectDB();
 */
export async function connectDB() {
  try {
    await client.connect();
    collection = client.db("WeatherStation").collection("SensorData");
    console.log("Verbonden met MongoDB!");
  } catch (error) {
    console.error("Fout bij verbinden:", error);
    throw error;
  }
}

/**
 * @function getCollection
 * @description Returns the active MongoDB collection instance.
 * Must be called after connectDB() has successfully established a connection.
 * @returns {import('mongodb').Collection} The SensorData collection instance
 * @throws {Error} If database connection has not been established
 * @example
 * const collection = getCollection();
 * await collection.find({}).toArray();
 */
export function getCollection() {
  if (!collection) {
    throw new Error("Database niet verbonden");
  }
  return collection;
}

/**
 * @async
 * @function closeDB
 * @description Gracefully closes the MongoDB client connection.
 * Should be called when shutting down the application.
 * @returns {Promise<void>}
 * @example
 * await closeDB();
 */
export async function closeDB() {
  await client.close();
}