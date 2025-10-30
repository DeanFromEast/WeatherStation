/**
 * @file api.js
 * @description Express router module that handles API endpoints for sensor data collection and retrieval.
 * Provides endpoints for receiving sensor measurements and fetching historical data for visualization.
 */

import express from 'express';
import { getCollection } from './database.js';

/**
 * @const {express.Router} router - Express router instance for API routes
 */
const router = express.Router();

/**
 * @route GET /data
 * @description Receives sensor data from weather station and stores it in the database.
 * Accepts temperature, humidity, and pressure values as query parameters.
 * @param {express.Request} req - Express request object
 * @param {number} req.query.temp - Temperature value in degrees Celsius
 * @param {number} req.query.hum - Humidity value in percentage
 * @param {number} req.query.pres - Atmospheric pressure value in hPa
 * @param {express.Response} res - Express response object
 * @returns {Promise<void>}
 * @throws {Error} If database insertion fails
 * @example
 * // Request: GET /data?temp=22.5&hum=65.3&pres=1013.2
 * // Response: { "success": true }
 */
router.get('/data', async (req, res) => {
  try {
    /**
     * @type {Object} data - Sensor data object to be stored
     * @property {number} temperature - Temperature in degrees Celsius
     * @property {number} humidity - Humidity percentage
     * @property {number} pressure - Atmospheric pressure in hPa
     * @property {Date} timestamp - Time when data was received
     */
    const data = {
      temperature: parseFloat(req.query.temp),
      humidity: parseFloat(req.query.hum),
      pressure: parseFloat(req.query.pres),
      timestamp: new Date()
    };
    console.log('Succesvol ontvangen data:', data);
    await getCollection().insertOne(data);
    res.status(200).json({ success: true });
  } catch (err) {
    console.error('Fout bij opslaan:', err);
    res.status(500).json({ success: false });
  }
});

/**
 * @route GET /fetch
 * @description Retrieves historical sensor data for chart visualization.
 * Supports filtering by time period (hour, day, or all data).
 * @param {express.Request} req - Express request object
 * @param {string} [req.query.period='all'] - Time period filter ('hour', 'day', or 'all')
 * @param {express.Response} res - Express response object
 * @returns {Promise<void>}
 * @throws {Error} If database query fails
 * @example
 * // Request: GET /fetch?period=day
 * // Response: {
 * //   "labels": ["01-01 12:00", "01-01 12:05", ...],
 * //   "values": [
 * //     { "temperature": 22.5, "humidity": 65.3, "pressure": 1013.2 },
 * //     ...
 * //   ]
 * // }
 */
router.get('/fetch', async (req, res) => {
  try {
    const period = req.query.period || 'all';
    
    /**
     * @type {Object} query - MongoDB query filter based on time period
     */
    let query = {};

    // Filter data based on requested time period
    if (period === 'hour') {
      query = { timestamp: { $gte: new Date(Date.now() - 60 * 60 * 1000) } };
    } else if (period === 'day') {
      query = { timestamp: { $gte: new Date(Date.now() - 24 * 60 * 60 * 1000) } };
    }

    /**
     * @type {Array<Object>} results - Array of sensor data documents from database
     */
    const results = await getCollection()
      .find(query)
      .sort({ timestamp: 1 })
      .limit(1000)
      .toArray();

    /**
     * @type {Array<string>} labels - Formatted timestamp labels for chart x-axis
     */
    const labels = results.map(entry =>
      new Date(entry.timestamp).toLocaleString('nl-NL', {
        hour: '2-digit',
        minute: '2-digit',
        day: '2-digit',
        month: '2-digit'
      })
    );

    /**
     * @type {Array<Object>} values - Array of sensor measurement objects for chart data
     */
    const values = results.map(entry => ({
      temperature: entry.temperature || 0,
      humidity: entry.humidity || 0,
      pressure: entry.pressure || 0
    }));

    res.json({ labels, values });
  } catch (err) {
    console.error('Fout bij ophalen:', err);
    res.status(500).json({ error: 'Fout bij ophalen data' });
  }
});

/**
 * @exports router
 * @description Express router with all API endpoints
 */
export default router;