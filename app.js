var sheet_id = "1RhakpUyaTxOUic7LScLXepM26NV0Uabx12J2UKlk1rA";
var sheet_name = "temp";

function doGet(e) {
    var ss = SpreadsheetApp.openById(sheet_id);
    var sheet = ss.getSheetByName(sheet_name);
    var temp = e.parameter.sensor;
    var date = e.parameter.date;
    var hum = e.parameter.hum;
    // sheet.appendRow([sensor,date]);
    sheet.appendRow([temp, hum, date]);
}