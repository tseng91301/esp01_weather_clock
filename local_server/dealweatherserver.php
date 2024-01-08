<?php
$hr=$_GET['hr'];
$hr=intval($hr);
$res_arr=json_decode(exec("curl 'http://api.weatherapi.com/v1/forecast.json?key=3a392efea7c04a888b3152040232108&q=Puli&days=3'"),true);
$out_arr=[];
$out_arr['weather_des']=$res_arr["forecast"]["forecastday"][0]["hour"][$hr]["condition"]["text"];
$out_arr['weather_temp_c']=strval($res_arr["forecast"]["forecastday"][0]["hour"][$hr]["temp_c"]);
$out_arr['weather_humidity']=strval($res_arr["forecast"]["forecastday"][0]["hour"][$hr]["humidity"]);
$out_arr['weather_chance_of_rain']=strval($res_arr["forecast"]["forecastday"][0]["hour"][$hr]["chance_of_rain"]);
$out_arr['weather_location']=$res_arr["location"]["name"].", ".$res_arr["location"]["region"];
echo(json_encode($out_arr));
?>
