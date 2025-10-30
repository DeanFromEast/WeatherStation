import { MongoClient, ServerApiVersion } from 'mongodb';

const uri = "mongodb+srv://Admin:Admin@cluster0.nx36qup.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";

const client = new MongoClient(uri, {
  serverApi: {
    version: ServerApiVersion.v1,
    strict: true,
    deprecationErrors: true,
  }
});

let collection;

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

export function getCollection() {
  if (!collection) {
    throw new Error("Database niet verbonden");
  }
  return collection;
}

export async function closeDB() {
  await client.close();
}