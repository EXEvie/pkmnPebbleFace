module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": ""
  },

  {
    "type": "section",
    "items":[
      {
        "type": "select",
      "messageKey": "pkmn_choice_in",
      "defaultValue": "--",
      "label": "Choose your starter!",
      "options": [
        { 
          "label": "", 
          "value": "" 
        },
        { 
          "label": "Bulbasaur",
          "value": 0 
        },
        { 
          "label": "Squirtle",
          "value": 1
        },
        { 
          "label": "Charmander",
          "value": 2 
        }
      ]
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];