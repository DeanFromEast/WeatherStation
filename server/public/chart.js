async function loadData() {
  const response = await fetch("/fetch");
  const json = await response.json();

  const ctx = document.getElementById("myChart");
  new Chart(ctx, {
    type: "bar",
    data: {
      labels: json.labels,
      datasets: [
        {
          label: "Temperatuur (°C)",
          data: json.values,
          backgroundColor: "rgba(54, 162, 235, 0.5)",
        },
      ],
    },
  });
}

loadData();
