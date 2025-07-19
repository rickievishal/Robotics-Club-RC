import logo from './logo.svg';
import './App.css';
import { useEffect, useState } from 'react';
import axios from 'axios';

function App() {
  const [ip, setip] = useState("")
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
        const response = axios.get(`http://${ip}/turn/off`)
      }catch(err){
        console.log(err);
      }
    }
  return (
    <div className="App py-16">
      <h1 className='text-2xl'>
        Robotics Club
      </h1>
      <p className='pt-16'>
        Ip address of your esp32 network
      </p>
      <input className='border px-4' placeholder='Enter the ip address' value={ip} onChange={(e) => setip(e.target.value)}/>
      <div>
        <p>
          LED IS {ledState ?  "on" : "off"}
        </p>
        <button className='px-4 py-1 rounded-lg bg-black text-white' onClick={()=> {
          if(ledState){
            turnOff()
          }else{
            turnOn()
          }
        }}>
            change led state
        </button>

      </div>
    </div>
  );
}

export default App;
