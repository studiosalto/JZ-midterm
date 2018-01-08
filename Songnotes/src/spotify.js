const spotifyLib = require('spotify-web-api-js');
const spotifyApi = new spotifyLib();
// spotify constants
const clientId = 'f7cf33c4220e4e6cb2acf0598ec33ade';
const clientSecret = 'b6493c35e2e448cf9b9fbc2ffb88eac0';
const baseUrl = 'https://api.spotify.com/v1';

//spotifyApi.setAccessToken(clientId);

export {
    clientId,
    clientSecret,
}

//authorize();

function authorize() {
    fetch('https://accounts.spotify.com/api/token', {
        method: 'post',
        body: JSON.stringify({
            "grant_type":    "authorization_code",
            //"code":          code,
            "redirect_uri":  'http://localhost:3000',
            "client_secret": clientSecret,
            "client_id":     clientId,
        })
    }).then(
        (resolved) => {
            console.log(resolved);
        },
        (rejected) => {
            console.log(rejected);
        }
    )
}

function _fetchRequest(url) {
    const myHeaders = new Headers();

    const init = {
        method: 'GET',
        headers: myHeaders,
        mode: 'cors',
        cache: 'default'
    };

    return fetch(url, init);
}

export function searchTracks(queryTerm) {
    return spotifyApi.searchTracks(queryTerm, {limit: 5})
}
