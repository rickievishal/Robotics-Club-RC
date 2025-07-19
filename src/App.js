import logo from './logo.svg';
import './App.css';
import { useEffect, useState } from 'react';
import axios from 'axios';

function App() {
  const [ip, setip] = useState("Your esp's ip address")
  const [ledState, setLedState] = useState(false)
  const turnOn = () => {
      try{
        const response = axios.get(`http://${ip}/turn/on`)
      }catch(err){
        console.log(err)
      }
    }
    const turnOff = () => {
      try{
        const response = axios.get(`http://${ip}/turn/on`)
      }catch(err){
        console.log(err)
      }
    }
  return (
    <div className="App">
      <input value={ip} onChange={(e) => setip(e.target.value)}/>
      
    </div>
  );
}

export default App;
