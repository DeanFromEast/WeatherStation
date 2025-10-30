import express from 'express';
import { connectDB, closeDB } from './database.js';
import apiRoutes from './api.js';

const app = express();
const PORT = 3000;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static("public"));

// Mount API routes
app.use('/', apiRoutes);

// Start server
async function startServer() {
  await connectDB();

  app.listen(PORT, () => {
    console.log(`Server draait op http://localhost:${PORT}`);
  });
}

startServer();

// Sluit netjes af
process.on('SIGINT', async () => {
  await closeDB();
  process.exit(0);
});