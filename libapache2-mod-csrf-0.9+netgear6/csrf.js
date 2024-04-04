//
// mod_csrf - Cross-site request forgery protection module for
//            the Apache web server
//
// Copyright (C) 2012 Christoph Steigmeier, Pascal Buchbinder
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//

// TODO: additonal types (some may contain javascript fragments)
var types = [
        ["a", "href"]
];

var isTimeOut = false;

// dynamic/ajax requests overriding the send function appending the id as a request header
function registerAjax(paramName) {

        XMLHttpRequest.prototype._open = XMLHttpRequest.prototype.open;
        XMLHttpRequest.prototype.open = function (method, url, async, user, pass) {
                this._open.apply(this, arguments);

                this.addEventListener("readystatechange", function () {

                        if (this.readyState == 4 && (this.status == 403) && !isTimeOut) {
                                isTimeOut = true;

                                if (!!~window.location.href.indexOf("wizard")) {
                                        document.getElementById("csrfIW").contentWindow.location.reload();
                                        return;
                                }

                                localStorage.setItem("is403LogOut", "1");
                                this.abort();

                                window.timeOutBox = Ext.UI.MessageBox.show({
                                        title: "Time Out",
                                        msg: "Time Out",
                                        buttons: Ext.Msg.OK,
                                        icon: Ext.MessageBox.WARNING,
                                        fn: function (id, msg) {
                                                window.location.reload();
                                        }
                                });

                        }

                } , false);

        }
        XMLHttpRequest.prototype._send = XMLHttpRequest.prototype.send;
        XMLHttpRequest.prototype.send = function (data) {

                if (!isTimeOut) {

                        if (this.onsend != null) {
                                this.onsend.apply(this, arguments);
                        }

                        this._send.apply(this, arguments);
                } else {
                        this.abort();
                }

        }
        XMLHttpRequest.prototype.onsend = function (data) {
                // add the id as a request header (don't want (know how) to modify the url)
                this.setRequestHeader(paramName, window.csrfId);
        }

};

// adds the csrfId to all known refernce nodes
function addToNodes(paramName) {
        var httpHost = "http://" + document.domain;
        var httpsHost = "https://" + document.domain;
        // iterate through all known types, e.g. "a"
        for (i = 0; i < types.length; i++) {
                var j;
                var name = types[i][0];
                // fetch all nodes from the document
                var nodes = document.getElementsByTagName(name);
                if (nodes != null) {
                        for (j = 0; j < nodes.length; j++) {
                                // process all attributes for these nodes, e.g. "href" for nodes of type "a"
                                var ai = 1;
                                while (types[i][ai] != null) {
                                        var attribute = nodes[j].getAttribute(types[i][ai]);
                                        // add id only once
                                        //        if((attribute != null) &&
                                        //           (attribute.indexOf(csrfId) == -1)) {
                                        // add id to every ref
                                        if (attribute != null) {
                                                var fragment = null;
                                                var newattribute;
                                                var update = false;
                                                if (attribute.indexOf("#") != -1) {
                                                        // extract the fragment and append it afterwards
                                                        fragment = attribute.substr(attribute.indexOf("#"));
                                                        attribute = attribute.substr(0, attribute.indexOf("#") - 1);
                                                }
                                                if (attribute.indexOf("?") == -1) {
                                                        // first query parameter
                                                        newattribute = attribute + "?" + paramName + "=" + window.csrfId;
                                                        // don't add id if it is not required (may disturb other js code)
                                                        // update = true;
                                                } else {
                                                        // append to existing query paramter
                                                        newattribute = attribute + "&" + paramName + "=" + window.csrfId;
                                                        update = true;
                                                }
                                                // absolute http reference
                                                if (attribute.match(/^http:\/\//)) {
                                                        if (attribute.indexOf(httpHost) == -1) {
                                                                // ignore link to other domain
                                                                update = false;
                                                        }
                                                }
                                                if (attribute.match(/^https:\/\//)) {
                                                        if (attribute.indexOf(httpsHost) == -1) {
                                                                // ignore link to other domain
                                                                update = false;
                                                        }
                                                }
                                                if (update) {
                                                        if (fragment) {
                                                                newattribute = newattribute + fragment;
                                                        }
                                                        nodes[j].setAttribute(types[i][ai], newattribute);
                                                }
                                        }
                                        ai++;
                                }
                        }
                }
        }
}

// adds the csrfId as a hidden field to every form
function addToForms(paramName) {
        var nodes = document.getElementsByTagName('form');
        for (var i = 0; i < nodes.length; i++) {
                var link = document.createElement('input');
                link.setAttribute('type', 'hidden');
                link.setAttribute('name', paramName);
                link.setAttribute('value', window.csrfId);
                nodes[i].appendChild(link);
        }
}

function csrfInsert(paramName, csrfId) {
        isTimeOut = false;
        window.csrfId = csrfId;

        // register callbacks when sending data by the browser
        registerAjax(paramName);

        // simple references
        addToNodes(paramName);

        // forms
        addToForms(paramName);
}

var throttle = function (fn, interval) {
        var __self = fn,
                timer, firstTime = false;
        return function () {
                var args = arguments,
                        __me = this;
                if (firstTime) {
                        __self.apply(__me, args);
                        return firstTime = false;
                }
                if (timer) {
                        return false;
                }
                timer = setTimeout(function () {
                        clearTimeout(timer);
                        timer = null;
                        __self.apply(__me, args);
                }, interval || 1000);
        };
};

var observeDOMChanges = (function () {
        var changedHandler = throttle(function (element) {
                var filterArr = ["FORM", "NOBR", "SPAN"];
                var tagName = element.tagName;

                if (isTimeOut) return;
                if (!!~element.className.indexOf("csrf-filter-ele") || !!~filterArr.indexOf(tagName)) return;
                document.getElementById("csrfIW").contentWindow.location.reload();
        });

        var MutationObserver = window.MutationObserver || window.WebKitMutationObserver || window.MozMutationObserver;

        var MutationObserverConfig = {
                childList: true,
                attributes: true,
                subtree: true
        };

        var observer = MutationObserver ? new MutationObserver(function (mutations) {

                mutations.forEach(function (mutation) {

                        changedHandler(mutation.target);

                        if (mutation.addedNodes) {
                                mutation.addedNodes.forEach(changedHandler);
                        }

                });

        }) : undefined;

        return observer ? function () {
                observer.observe(document.body, MutationObserverConfig);
        } : function () {
                console.log('MutationObserver not support!');
        };
})();

window.onload = function () {
        observeDOMChanges();

        if (!!~window.location.href.indexOf("wizard")) {

                window.wizardIntervalID = window.setInterval(function () {
                        document.getElementById("csrfIW").contentWindow.location.reload();
                }, 60000);

        }

}

window.onbeforeunload = function () {
        window.wizardIntervalID && window.clearInterval(window.wizardIntervalID);
}
