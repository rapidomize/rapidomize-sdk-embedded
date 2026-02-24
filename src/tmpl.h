#ifndef RPZ_TMPL_H_
#define RPZ_TMPL_H_

#include <Arduino.h>

namespace rpz{

const char *main_tmpl = R"(
<!DOCTYPE html>
<html lang='en'>
    <head>
        <meta name='viewport' content='width=device-width'>
        <title>Rapidomize IoT Edge</title>
        <style>
            h3{
                margin-top: 10px;
            }
            .fx{
                display: flex;
            }
            .fx-g{
                flex: 1 1 auto;
            }
            .row{
                display: flex;
                flex-direction: row;
            }
            .column{
                display: flex;
                flex-direction: column;
            }
            .container{
                max-width: 800px;
                margin: auto;
                padding: 10px 20px;
            }
            .pos-r{
                position: relative;
            }
            .pos-a{
                position: absolute;
            }
            .card{
                border: 1px solid gray; 
                border-radius: 6px;
                padding: 10px;
                min-width: fit-content;
            }
            .cards{
                display: grid;
                grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
                grid-gap: 10px;
            }
            .g-10{
                grid-gap: 10px;
            }
            .brdr{
                border-radius: 6px;
                padding: 10px;
            }
            .mt-10{
                margin-top: 10px;
            }
            .mt-30{
                margin-top: 30px;
            }
            .tabs {
                display: flex;
                flex-wrap: wrap;
                font-family: sans-serif;
            }
            .tabs__label {
                padding: 10px 16px;
                cursor: pointer;
            }
            .tabs__radio {
                display: none;
            }
            .tabs__content {
                order: 1;
                width: 100%%;
                height: 100%%;
                border-top: 2px solid #dddddd;
                display: none;
            }
            .tabs__radio:checked + .tabs__label {
                font-weight: bold;
                color: #37a000;
                border-bottom: 3px solid #37a000;
            }
            .tabs__radio:checked + .tabs__label + .tabs__content {
                display: initial;
            }
            input, select{
                padding: 4px 12px;
                font-size: 14px;
                border: 1px solid gray;
                border-radius: 6px;
                transition: border-color 0.15s ease-in-out, box-shadow 0.15s ease-in-out;
                min-width: fit-content;
                width: auto;
            }
            .switch { position: relative; display: inline-block; width: 40px; height: 14px; }
            .switch input { opacity: 0; width: 0; height: 0; }
            .slider {
                position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0;
                background-color: #ccc; transition: .2s; border-radius: 34px;
            }
            .slider:before {
                position: absolute; content: ""; height: 22px; width: 22px;
                left: 0px; bottom: -5px; 
                transition: .2s; border-radius: 50%%;
                background-color: #eee;
                border: 1px solid #eee;
                box-shadow: 0 2px 4px rgba(0,0,0,0.2);
            }
            .switch input:checked + .slider { background-color: #37a000; }
            .switch input:checked + .slider:before { background-color: #fff; border-color: #37a000; transform: translateX(22px); }
            input[type=checkbox]{
                display: block;
                width: 1em;
                height: 1em;
                margin-top: 0.25em;
                background-color: #fff;
                background-repeat: no-repeat;
                background-position: center;
                background-size: contain;
                -webkit-appearance: none;
                -moz-appearance: none;
                appearance: none;
                -webkit-print-color-adjust: exact;
            }
            input[type=checkbox]:checked {
                background-color:  #37a000;
                border-color: #37a000;
                background-image: url("data:image/svg+xml,%%3csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%%3e%%3cpath fill='none' stroke='%%23fff' stroke-linecap='round' stroke-linejoin='round' stroke-width='3' d='M6 10l3 3l6-6'/%%3e%%3c/svg%%3e");
            }
        </style>
    </head>
    <body class="container column">
        <div class="row" style="align-items: end; margin-bottom: 20px;">
            <img class="img-fluid" src="https://rapidomize.io/img/logo-text.svg" alt="Rapidomize | Low-Code Service Development Platform" loading="lazy" 
                style="height:auto;max-width: 100%%; max-height: 56px;margin-right: 30px;">
            <h1>IoT Edge</h1>
        </div>
        <!-- messages (if any) -->
        <div  id="msg">%s</div>
        <div class="tabs">
            %s
            %s
            %s
            %s
            <input type="radio" class="tabs__radio" name="atabs" id="tab5">
            <label for="tab5" class="tabs__label">Firmware</label>
            <div class="tabs__content">
                <h2>Firmware Upgrade</h2>
                <p>Upgrade firmware using a local file or remote url.</p>
                <form action="/fwurl" method="post" class="card column brd">
                    <h4>Use A Remote URL (OTA)</h4>
                    <input type="text" name="fw_url" value="https://" class="fx-g">
                    <input type="submit"  value="Update" class="brdr" style="margin: 20px auto; width: 200px;">
                </form>
                <form action="/fwfile" method="post" class="card column brd mt-30">
                    <h4>Use A Local File</h4>
                    <input type="file" name="fw_file">
                    <input type="submit"  value="Update" class="brdr" style="margin: 20px auto; width: 200px;">
                </form>
            </div>
            <input type="radio" class="tabs__radio" name="atabs" id="tab6">
            <label for="tab6" class="tabs__label">Logs</label>
            <div class="tabs__content">
                <h2>Logs</h2>
                <div id="evts" class="card column" style="padding-left: 20px;height: 200px;overflow-y: auto;"></div>
            </div>
            <input type="radio" class="tabs__radio" name="atabs" id="tab7">
            <label for="tab7" class="tabs__label">Reset</label>
            <div class="tabs__content">
                <h2>Reset</h2>
                <p>Reset the IoT Edge to it's factory settings</p>
                <form id="reset" action="/reset" method="post" class="column mt-30">
                    <input type="submit" value='Factory Reset' class="brdr" style="margin: 20px auto; width: 200px;">
                </form>
            </div>
        </div>
        <script>
            
            const forms = document.getElementsByTagName('form');
            const msg = document.getElementById('msg');
            for(let i=0; i < forms.length; i++){
                if(forms[i].action.startsWith("/fw")) continue;
                forms[i].addEventListener('submit', function (event) {
                    event.preventDefault();
                    event.stopPropagation();
                    const form = event.target;
                    // Convert form data to a JavaScript object using the FormData API
                    const frm = new FormData(form);
                    const frmData = Object.fromEntries(frm.entries());
                    console.log('data', frmData);

                    // Convert the JavaScript object to a JSON string
                    const jdata = JSON.stringify(frmData);

                    fetch(form.action, {
                        method: 'POST', 
                        headers: {
                        'Content-Type': 'application/json' 
                        },
                        body: jdata,
                        signal: AbortSignal.timeout(3000) 
                    }).then(response => {
                        if (!response.ok) {
                            const contentType = response.headers.get("content-type");
                            if (!contentType || !contentType.includes("application/json")) {
                                throw new Error(`Unexpected error! status: ${response.status}`);
                            }
                        }
                        return response.json()
                    }).then(data => {
                        console.log('Success:', data);
                        if(data['err']){
                            msg.textContent = data['err'];
                            msg.setAttribute('style', 'color: red;');
                        }else {
                            msg.textContent = 'Success';
                            msg.setAttribute('style', 'color: green;');
                        }
                        //window.location.href = '/';
                    }).catch((error) => {
                        console.error('Error:', error);
                        msg.textContent = error;
                        msg.setAttribute('style', 'color: red;');
                    });
                });
            }
            const evtSource = new EventSource("/evts");
            evtSource.onmessage = (event) => {
                const eventList = document.getElementById("evts");
                if(eventList.children.length > 100)
                    eventList.firstChild.remove();
                const newElement = document.createElement("li");
                newElement.textContent = event.data;
                eventList.appendChild(newElement);
            };
            evtSource.onerror = (err) => {
                console.error("EventSource failed:", err);
            };
        </script>
    </body>
</html>
)";

const char *dash_tmpl = R"(
<input type="radio" class="tabs__radio" name="atabs" id="tab1" checked>
<label for="tab1" class="tabs__label">Home</label>
<div class="tabs__content">
    <table class="card mt-10">
        <tr><td>Model</td><td>%s</td></tr>
        <tr><td>Firmware Version</td><td>%s</td></tr>
        <tr><td style="width: 150px;">CPU Freq</td><td>%dMHz</td></tr>
        <tr><td>Image Size</td><td>%dKB</td></tr>
        <tr><td>IP Address</td><td>%s</td></tr>
    </table>
    <div class="cards mt-10">
        %s
    </div>
</div>
)";

const char *dash_fr_tmpl = R"(
<div class="card">%s</div>
)";

const char *wifi_tmpl = R"(
<input type="radio" class="tabs__radio" name="atabs" id="tab2">
<label for="tab2" class="tabs__label">WiFi</label>
<div class="tabs__content">
    <div class="row mt-10"><div style="margin-right: 10px;">IP:</div><div>%s</div></div>
    <form action="/wifi" method="post" class="column">
        <h2>Available WiFi Networks</h2>
        <p>Select a WiFi network and provide it's credentials</p>
        <label class="card" style="padding: 10px;">SSIDs:
            <div class="column">
                %s
            </div>
        </label>
        <label style="margin-top: 10px;">WiFi password:
            <input type="password" name="pwd">
        </label>
        <input type="submit"  value="Connect" class="brdr" style="margin: 20px auto; width: 200px;">
    </form>
</div>
)";

const char *mqtt_tmpl = R"(
<input type="radio" class="tabs__radio" name="atabs" id="tab3">
<label for="tab3" class="tabs__label">MQTT</label>
<div class="tabs__content">
    <form action="/mqtt" method="post" class="column">
        <h2>MQTT Broker</h2>
        <p>Specify MQTT Broker details.</p>
        <div class="column" style="grid-gap: 5px;">
            <table class="card">
                <tr><td>Host</td><td class="fx"><input type="text" name="host" value="%s" style="flex-grow: 1"></td></tr>
                <tr><td>Port</td><td><input type="number" name="port" value="%d"></td></tr>
                <tr><td>Client ID</td><td class="fx"><input type="text" name="clientId" value="%s"  style="flex-grow: 1"></td></tr>
                <tr><td>Username</td><td class="fx"><input type="text" name="username" value="%s"  style="flex-grow: 1"></td></tr>
                <tr><td>Password</td><td><input type="password" name="password" value="%s"></td></tr>
                <tr><td style="width: 150px;">Publishing Topic</td><td class="fx"><input type="text" name="topic"  value="%s" style="flex-grow: 1"></td></tr>
                <tr><td>TLS/SSL</td><td><input type="checkbox" name="tls" disabled %s></td></tr>
                <tr><td>Version</td><td>
                    <select name="ver" value="%s">
                        <option value="3.1.1">3.1.1</option>
                        <option value="5.0">5.0</option>
                    </select>
                </td></tr>
                <tr><td>QoS</td><td>
                    <select name="qos" value="%d">
                        <option value="0">0</option>
                        <option value="1">1</option>
                        <option value="2">2</option>
                    </select>  
                </td></tr>
            </table>
        </div>
        <input type="submit" value="Connect" class="brdr" style="margin: 20px auto; width: 200px;">
    </form>
</div>
)";

const char *peri_tmpl = R"(
<input type="radio" class="tabs__radio" name="atabs" id="tab4">
<label for="tab4" class="tabs__label">Peripherals</label>
<div class="tabs__content">
    <form action="/peri" method="post" class="column">
        <h2>Peripherals</h2>
        <p>Peripheral hardware configuration</p>
        <div class="cards g-10">
            %s
        </div>

        <input type="submit" value="Save" class="brdr" style="margin: 20px auto; width: 200px;">
    </form>
</div>
)";

const char *ssid_tmpl = R"(<div><input type="radio" name="ssid" value="%s" %s> <label>%s</label></div>)";
const char *success_pg = R"(
<!DOCTYPE html><html lang='en'>
<head>
    <meta name='viewport' content='width=device-width'>
    <title>Rapidomize IoT Edge</title>
</head>
<body>Thanks</body></html>
)";    


} // namespace rpz




#endif //RPZ_TMPL_H_