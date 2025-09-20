<script setup lang="ts">
import { ref } from 'vue'
import Card from './components/Card.vue';
import HelloWorld from './components/HelloWorld.vue';

const cardList = ref([
  { id: 0, title: 'Temperature', location: 'Sao Paulo', temperature: 25, humidity: 60},
  { id: 1, title: 'Temperature', location: 'Sao Paulo', temperature: 30, humidity: 50},
  { id: 2, title: 'Temperature', location: 'Sao Paulo', temperature: 35, humidity: 30},
])

</script>

<template>
  <div>
    <HelloWorld />
  </div>
  <br />
  <div class="flex flex-nowrap gap-4">
    <Card v-for="item in cardList" v-bind="item" :key="item.id" />
    <!-- <Card v-for="item in cardList" :title="item.title" :location="item.location" :temperature="item.temperature" :humidity="item.humidity" :key="item.id" /> -->
  </div>
</template>
