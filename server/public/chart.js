/**
 * @file chart.js
 * @description Client-side JavaScript for weather station data visualization.
 * Handles chart creation, data fetching, statistics calculation, and real-time updates.
 */

/**
 * @type {Object.<string, Chart>}
 * @description Store for Chart.js instances indexed by chart ID
 */
let charts = {};

/**
 * @type {string}
 * @description Current time period filter ('hour', 'day', or 'all')
 * @default 'all'
 */
let currentPeriod = 'all';

/**
 * @function calculateStats
 * @description Calculates minimum, maximum, and average values from a data array.
 * @param {Array<number>} data - Array of numerical values
 * @returns {Object} Statistics object
 * @returns {number} returns.min - Minimum value in the dataset
 * @returns {number} returns.max - Maximum value in the dataset
 * @returns {number} returns.avg - Average value of the dataset
 * @example
 * const stats = calculateStats([20, 22, 24, 21]);
 * // Returns: { min: 20, max: 24, avg: 21.75 }
 */
function calculateStats(data) {
  if (data.length === 0) return { min: 0, max: 0, avg: 0 };
  const min = Math.min(...data);
  const max = Math.max(...data);
  const avg = data.reduce((a, b) => a + b, 0) / data.length;
  return { min, max, avg };
}

/**
 * @async
 * @function loadData
 * @description Fetches sensor data from the server and updates all charts and statistics.
 * Retrieves data based on the current time period filter, calculates statistics,
 * updates DOM elements with current values and stats, and refreshes all charts.
 * @returns {Promise<void>}
 * @throws {Error} If fetch request fails or data processing encounters an error
 */
async function loadData() {
  try {
    const response = await fetch(`/fetch?period=${currentPeriod}`);
    const json = await response.json();

    if (!json.labels || json.labels.length === 0) {
      document.getElementById('loading').textContent = 'Geen data beschikbaar';
      return;
    }

    /**
     * @type {Array<string>} labels - Formatted timestamp labels for chart x-axis
     */
    const labels = json.labels;

    /**
     * @type {Array<number>} temperatures - Temperature values in degrees Celsius
     */
    const temperatures = json.values.map(v => v.temperature);

    /**
     * @type {Array<number>} humidities - Humidity values in percentage
     */
    const humidities = json.values.map(v => v.humidity);

    /**
     * @type {Array<number>} pressures - Pressure values in hPa
     */
    const pressures = json.values.map(v => v.pressure);

    // Update last update time
    document.getElementById('last-update').textContent =
      'Laatste update: ' + new Date().toLocaleTimeString('nl-NL');

    // Calculate statistics
    const tempStats = calculateStats(temperatures);
    const humStats = calculateStats(humidities);
    const presStats = calculateStats(pressures);

    // Update current values
    const lastIndex = temperatures.length - 1;
    document.getElementById('temp-current').textContent = temperatures[lastIndex]?.toFixed(1) || '--';
    document.getElementById('hum-current').textContent = humidities[lastIndex]?.toFixed(1) || '--';
    document.getElementById('pres-current').textContent = pressures[lastIndex]?.toFixed(1) || '--';

    // Update temperature stats
    document.getElementById('temp-min').textContent = tempStats.min.toFixed(1) + '°C';
    document.getElementById('temp-max').textContent = tempStats.max.toFixed(1) + '°C';
    document.getElementById('temp-avg').textContent = tempStats.avg.toFixed(1) + '°C';
    document.getElementById('temp-range').textContent = `Range: ${tempStats.min.toFixed(1)}°C - ${tempStats.max.toFixed(1)}°C`;
    document.getElementById('temp-count').textContent = `${temperatures.length} metingen`;

    // Update humidity stats
    document.getElementById('hum-min').textContent = humStats.min.toFixed(1) + '%';
    document.getElementById('hum-max').textContent = humStats.max.toFixed(1) + '%';
    document.getElementById('hum-avg').textContent = humStats.avg.toFixed(1) + '%';
    document.getElementById('hum-range').textContent = `Range: ${humStats.min.toFixed(1)}% - ${humStats.max.toFixed(1)}%`;
    document.getElementById('hum-count').textContent = `${humidities.length} metingen`;

    // Update pressure stats
    document.getElementById('pres-min').textContent = presStats.min.toFixed(1) + ' hPa';
    document.getElementById('pres-max').textContent = presStats.max.toFixed(1) + ' hPa';
    document.getElementById('pres-avg').textContent = presStats.avg.toFixed(1) + ' hPa';
    document.getElementById('pres-range').textContent = `Range: ${presStats.min.toFixed(1)} - ${presStats.max.toFixed(1)} hPa`;
    document.getElementById('pres-count').textContent = `${pressures.length} metingen`;

    // Create/update charts
    createOrUpdateChart('temp', labels, temperatures, 'Temperatuur (°C)', 'rgb(220, 53, 69)', 'rgba(220, 53, 69, 0.1)');
    createOrUpdateChart('hum', labels, humidities, 'Luchtvochtigheid (%)', 'rgb(13, 110, 253)', 'rgba(13, 110, 253, 0.1)');
    createOrUpdateChart('pres', labels, pressures, 'Luchtdruk (hPa)', 'rgb(25, 135, 84)', 'rgba(25, 135, 84, 0.1)');

    document.getElementById('loading').style.display = 'none';
    document.getElementById('content').style.display = 'block';
  } catch (error) {
    console.error('Error loading data:', error);
    document.getElementById('loading').textContent = 'Fout bij het laden van data: ' + error.message;
  }
}

/**
 * @function createOrUpdateChart
 * @description Creates a new Chart.js line chart or updates an existing one with new data.
 * If a chart with the given ID already exists, it updates the data and refreshes the chart.
 * Otherwise, it creates a new chart with the specified configuration.
 * @param {string} id - Canvas element ID where the chart should be rendered
 * @param {Array<string>} labels - Array of x-axis labels (timestamps)
 * @param {Array<number>} data - Array of y-axis data points
 * @param {string} label - Dataset label displayed in the legend
 * @param {string} borderColor - CSS color string for the line border
 * @param {string} backgroundColor - CSS color string for the area fill
 * @returns {void}
 * @example
 * createOrUpdateChart('temp', ['12:00', '12:05'], [22.5, 23.1], 
 *   'Temperatuur (°C)', 'rgb(220, 53, 69)', 'rgba(220, 53, 69, 0.1)');
 */
function createOrUpdateChart(id, labels, data, label, borderColor, backgroundColor) {
  const ctx = document.getElementById(id);

  if (charts[id]) {
    // Update existing chart
    charts[id].data.labels = labels;
    charts[id].data.datasets[0].data = data;
    charts[id].update();
  } else {
    // Create new chart
    charts[id] = new Chart(ctx, {
      type: "line",
      data: {
        labels: labels,
        datasets: [{
          label: label,
          data: data,
          borderColor: borderColor,
          backgroundColor: backgroundColor,
          fill: true,
          tension: 0.1,
          pointRadius: 2,
          pointHoverRadius: 5,
          borderWidth: 2
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
          legend: {
            display: true,
            position: 'top',
            labels: {
              font: {
                size: 12,
                family: 'Arial'
              }
            }
          },
          tooltip: {
            mode: 'index',
            intersect: false
          }
        },
        scales: {
          y: {
            beginAtZero: false,
            grid: {
              color: 'rgba(0, 0, 0, 0.1)',
              drawBorder: true
            },
            ticks: {
              font: {
                size: 11
              }
            }
          },
          x: {
            grid: {
              display: true,
              color: 'rgba(0, 0, 0, 0.05)'
            },
            ticks: {
              maxRotation: 45,
              minRotation: 45,
              font: {
                size: 10
              }
            }
          }
        },
        interaction: {
          mode: 'nearest',
          axis: 'x',
          intersect: false
        }
      }
    });
  }
}

/**
 * @event click
 * @description Time filter button click event handler.
 * Updates the active filter button styling and reloads data for the selected time period.
 * @listens .time-filter button#click
 */
document.querySelectorAll('.time-filter button').forEach(button => {
  button.addEventListener('click', (e) => {
    document.querySelectorAll('.time-filter button').forEach(btn =>
      btn.classList.remove('active')
    );
    e.target.classList.add('active');
    currentPeriod = e.target.dataset.period;
    loadData();
  });
});

/**
 * @event load
 * @description Window load event handler that triggers initial data load.
 * @listens window#load
 */
window.addEventListener("load", loadData);

/**
 * @const {number} AUTO_REFRESH_INTERVAL
 * @description Auto-refresh interval in milliseconds (10 seconds)
 * @default 10000
 */
// Auto-refresh every 10 seconds
setInterval(loadData, 10000);