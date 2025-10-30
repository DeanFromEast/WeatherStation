/**
 * @file index.js
 * @description Express server application that handles API routes and database connections.
 * Sets up middleware for JSON parsing, URL encoding, and static file serving.
 */

import express from 'express';
import { connectDB, closeDB } from './database.js';
import apiRoutes from './api.js';

/**
 * @const {express.Application} app - Express application instance
 */
const app = express();

/**
 * @const {number} PORT - Port number on which the server listens
 * @default 3000
 */
const PORT = 3000;

// Middleware configuration
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static("public"));

/**
 * Mount API routes at the root path
 */
app.use('/', apiRoutes);

/**
 * @async
 * @function startServer
 * @description Initializes database connection and starts the Express server.
 * Connects to the database before listening on the specified port.
 * @returns {Promise<void>}
 * @throws {Error} If database connection or server startup fails
 */
async function startServer() {
  await connectDB();

  app.listen(PORT, () => {
    console.log(`Server draait op http://localhost:${PORT}`);
  });
}

// Start the server
startServer();

/**
 * @event SIGINT
 * @description Gracefully shuts down the server on interrupt signal (Ctrl+C).
 * Closes database connection before exiting the process.
 * @listens process#SIGINT
 */
process.on('SIGINT', async () => {
  await closeDB();
  process.exit(0);
});