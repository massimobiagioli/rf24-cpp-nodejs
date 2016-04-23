{
  "targets": [
    {
      "target_name": "RF24Addon",
      "sources": [ "RF24Addon.cpp", "RF24Wrapper.cpp" ],
      "include_dirs": [ "RF24" ],
      "library_dirs": [ "RF24" ],
      "libraries": [ "-lrf24-bcm" ]      
    }
  ]
}
