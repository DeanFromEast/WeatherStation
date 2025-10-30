let charts = {};
let currentPeriod = 'all';

function calculateStats(data) {
  if (data.length === 0) return { min: 0, max: 0, avg: 0 };
  const min = Math.min(...data);
  const max = Math.max(...data);
  const avg = data.reduce((a, b) => a + b, 0) / data.length; 
  return { min, max, avg };
}

async function loadData() {
  try {
    const response = await fetch(`/fetch?period=${currentPeriod}`);
    const json = await response.json();

    if (!json.labels || json.labels.length === 0) {
      document.getElementById('loading').textContent = 'Geen data beschikbaar';
      return;
    }

    const labels = json.labels;
    const temperatures = json.values.map(v => v.temperature);
    const humidities = json.values.map(v => v.humidity);
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

function createOrUpdateChart(id, labels, data, label, borderColor, backgroundColor) {
  const ctx = document.getElementById(id);

  if (charts[id]) {
    charts[id].data.labels = labels;
    charts[id].data.datasets[0].data = data;
    charts[id].update();
  } else {
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

// Time filter buttons
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

window.addEventListener("load", loadData);

// Auto-refresh every 10 seconds
setInterval(loadData, 10000);