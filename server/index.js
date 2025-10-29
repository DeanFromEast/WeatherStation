import express from 'express';
import { MongoClient, ServerApiVersion } from 'mongodb';
const app = express();
const PORT = 3000;

const uri = "mongodb+srv://Admin:Admin@cluster0.nx36qup.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";
let db;
let collection;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

const client = new MongoClient(uri, {
  serverApi: {
    version: ServerApiVersion.v1,
    strict: true,
    deprecationErrors: true,
  }
});

async function run() {
  try {
    await client.connect();
    // await client.db("admin").command({ ping: 1 });
    await client.db("WeatherStation").command({ ping: 1 });

    db = client.db("WeatherStation");
    collection = db.collection("SensorData");
    console.log("Pinged your deployment. You successfully connected to MongoDB!");
  } finally {
    await client.close();
  }
}
run().catch(console.dir);

app.use(express.static("public"));

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});


// params
app.get('/data', (req, res) => {


  const temperature = req.query.temp;
  const humidity = req.query.hum;
  const pressure = req.query.pres;

  console.log(`Temperature: ${temperature}, Humidity: ${humidity}, Pressure: ${pressure}`);



  // Store in database (MongoDB example)

  client.connect().then(() => {
    const data = {
      temperature: parseFloat(temperature),
      humidity: parseFloat(humidity),
      pressure: parseFloat(pressure),
      timestamp: new Date()
    };
    collection.insertOne(data).then(() => {
      console.log('Data stored in database');
      client.close();
    }).catch(err => {
      console.error('Error storing data:', err);
      client.close();
    });
  }).catch(err => {
    console.error('Error connecting to database:', err);
  });
});


// Interface endpoint to view data with the use of chart.js
app.get('/fetch', (req, res) => {

  client.connect().then(() => {

    let labels = [];
    let values = [];


    collection.find().sort({ timestamp: -1 }).limit(10).toArray().then(results => {
      labels = results.map(entry => new Date(entry.timestamp).toLocaleTimeString());
      values = results.map(entry => entry.temperature);
      res.json({ labels, values });
    }).catch(err => {
      console.error('Error fetching data:', err);
      res.status(500).send('Error fetching data');
    });

    const data = {
      labels,
      values
    };
    // res.json(data);
  }).catch(err => {
    console.error('Error connecting to database:', err);
    res.status(500).send('Error connecting to database');
  });
});


// test api endpoint
app.get('/test', (req, res) => {
  res.json({ message: 'API is working!' });
  res.status(200);
  console.log('API PING!!!!!')
});

// app.get('/', (req, res) => {
//   res.json({ message: 'API is working!' });
//   // response 200 ok
//   res.status(200);
//   console.log('API PING!!!!! from /')
// });
