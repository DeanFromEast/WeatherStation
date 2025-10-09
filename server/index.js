import express from 'express';
const app = express();
const PORT = 3000;

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});

app.post('/data', (req, res) => {
  // Handle incoming data from STM32
  res.send('Data received');
  console.log('Data received:');
  console.log(req.body);
});


// params
app.get('/test/:param1', (req, res) => {
  res(req.params.param1 );
  console.log(param1, param2);
});
