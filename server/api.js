import express from 'express';
import { getCollection } from './database.js';

const router = express.Router();

// Ontvang sensor data
router.get('/data', async (req, res) => {
  try {
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

// Haal data op voor grafieken
router.get('/fetch', async (req, res) => {
  try {
    const period = req.query.period || 'all';
    let query = {};

    if (period === 'hour') {
      query = { timestamp: { $gte: new Date(Date.now() - 60 * 60 * 1000) } };
    } else if (period === 'day') {
      query = { timestamp: { $gte: new Date(Date.now() - 24 * 60 * 60 * 1000) } };
    }

    const results = await getCollection()
      .find(query)
      .sort({ timestamp: 1 })
      .limit(1000)
      .toArray();

    const labels = results.map(entry =>
      new Date(entry.timestamp).toLocaleString('nl-NL', {
        hour: '2-digit',
        minute: '2-digit',
        day: '2-digit',
        month: '2-digit'
      })
    );

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

export default router;