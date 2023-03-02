<template>
  <h1 class = "header">Python compiler</h1>
  <p class = "subtext">Created by Daniel Skymoen</p>
  <div class = "comp">
    <textarea class = "code box" v-model = "code"></textarea>
    <button>Compile</button>
    <textarea readonly class = "compiled box"></textarea>
  </div>
</template>

<style>
.subtext {
font-style: italic;
}
.comp {
  display: inline-flex;
  justify-content: center;
  vertical-align: center;
  padding-top: 30px;
  gap: 10px;
}
.box {
  width: 600px;
  height: 720px;
  font-size: 130%;
  resize: none;
}
button {
  height: 30px;
}
</style>
<script>

import CompilerService from "@/services/CompilerService";
 export default {
   name: "CompilerComponent",
   data() {
     return {
       code:"Input your python code here!",
       compiled:"",
     }
   },
   methods: {
     compile(){
       CompilerService.sendCode(this.code)
           .then((response) => {
             let x = response.toString();
             this.code = x;
           })
           .catch((error) => {
             let x = error.toString();
             this.code = x;
           })
     }
   }
 };

</script>