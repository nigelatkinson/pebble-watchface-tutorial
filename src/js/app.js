// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    //console.log('AppMessage received!');
    getWeather();
    getNews();
  }
);

function requestFailed(evt) {
  console.log("An error occurred while requesting news/weather.");
}

function requestCanceled(evt) {
  console.log("News/weather request was aborted.");
}

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.addEventListener("error", requestFailed);
  xhr.addEventListener("abort", requestCanceled);
  xhr.open(type, url);
  xhr.send();
};

/**
 * Fetch weather information if the location call is successful
 */
function locationSuccess(pos) {
  var myAPIKey = '68820190288601c486012aad2f9f7d9f';
  
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      //console.log('Name is ' + json.name);
      var location = json.name;
      
      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      //console.log('Temperature is ' + temperature);

      // Conditions
      var conditions = json.weather[0].main;      
      //console.log('Conditions are ' + conditions);
      
        // Assemble dictionary using our keys
      var dictionary = {
        'KEY_TEMPERATURE': temperature,
        'KEY_CONDITIONS': conditions,
        'KEY_LOCATION' : location
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          //console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending weather info to Pebble!');
        }
      );
      
    }      
  );
  
}

/**
 * Location error handler
 */
function locationError(err) {
  console.log('Error requesting location!');
}

/**
 * Get weather information from OpenWeather
 */
function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

/**
 * Get news headlines from the BBC UK news RSS feed.
 */
function getNews()
{
  //console.log("Getting news headlines...")
  // Construct URL
  var url = 'http://feeds.bbci.co.uk/news/rss.xml?edition=uk';

  var x = new XMLHttpRequest();
  x.addEventListener("error", requestFailed);
  x.addEventListener("abort", requestCanceled);
  x.open("GET", url, true);
  x.setRequestHeader("Cache-Control", "no-cache");
  x.setRequestHeader("Pragma", "no-cache");
  x.onreadystatechange = function () {
    if (x.readyState == 4 && x.status == 200)
    {
      if (x.responseText !== null)
      {
        //console.log(x.responseText);
        var matchHeadlineRegexp = /<item>\s*<title><!\[CDATA\[(.*)\]\]><\/title>/g;
        var titles = new Array();
        var i = 1;
        
        // Extract max 6 headlines
        while ((titles[i] = matchHeadlineRegexp.exec(x.responseText)) && i < 7)
        {
            //console.log(titles[i][1]);
            i++;
        }
      
        // Assemble dictionary using our keys
        var dictionary = {
          'KEY_HEADLINE_1': titles[1][1],
          'KEY_HEADLINE_2': titles[2][1],
          'KEY_HEADLINE_3': titles[3][1],
          'KEY_HEADLINE_4': titles[4][1],
          'KEY_HEADLINE_5': titles[5][1],
          'KEY_HEADLINE_6': titles[6][1]
        };

        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
          function(e) {
            //console.log('News sent to Pebble successfully!');
          },
          function(e) {
            console.log('Error sending news to Pebble!');
          }
        );
      }else{
        console.log('Error: No response from url '+url);
      }
    }
  };
  x.send(); 
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    //console.log('PebbleKit JS ready!');

    // Get the initial weather
    getWeather();
    getNews();
  }
);