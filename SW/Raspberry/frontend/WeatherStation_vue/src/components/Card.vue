<script>
export default {
  // props: ['title', 'location', 'temperature', 'humidity']
  props: {
    title: {
      type: String,
      required: true
    },
    location: {
      type: String,
      required: true
    },
    temperature: {
      type: Number,
      required: true
    },
    humidity: {
      type: Number,
      required: true
    }
  }
}
</script>


<template>
<div class="flex justify-center items-center min-h-screen bg-gray-100">
    <div class="max-w-sm rounded-lg overflow-hidden shadow-lg bg-white p-6">
      <div class="flex items-center space-x-4">
        <!-- Weather Icon -->
        <div class="p-3 bg-blue-200 rounded-full">
          <i class="fas fa-sun text-yellow-500 text-3xl"></i> <!-- Sun icon for sunny weather -->
        </div>

        <!-- Temperature & Location Info -->
        <div>
          <h2 class="text-4xl font-bold text-gray-800">{{temperature}}°C</h2> <!-- Current temperature -->
          <p class="text-sm text-gray-500">{{location}}</p> <!-- Weather description and location -->
        </div>
      </div>

      <div class="mt-6">
        <!-- Weather Details -->
        <div class="flex justify-between text-gray-600 text-sm">
          <div class="flex items-center space-x-2">
            <i class="fas fa-thermometer-half text-red-400"></i>
            <span>{{maxTemperature}}</span>
          </div>
          <div class="flex items-center space-x-2">
            <i class="fas fa-thermometer-empty text-blue-400"></i>
            <span>{{minTemperature}}</span>
          </div>
        </div>
        
        <!-- Humidity -->
        <div class="mt-2 flex items-center space-x-2 text-gray-600 text-sm">
          <i class="fas fa-tint text-blue-500"></i>
          <span>Humidity: {{humidity}}%</span>
        </div>
      </div>

      <div class="mt-4 text-center">
        <button class="bg-blue-500 text-white px-6 py-2 rounded-full hover:bg-blue-600 focus:outline-none">Refresh</button>
      </div>
    </div>
  </div>

</template>

<style scoped>
</style>