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
      "defaultValue": "0",
      "label": "Choose your starter!",
      "options": [
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
      },
    {
      "type": "select",
      "messageKey": "date_type",
      "defaultValue":"--",
      "label":"Choose date format",
      "options":[
        {
          "label":"DD/MM",
          "value":0
        },
        {
          "label":"MM/DD",
          "value":1
        }
        ]
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];