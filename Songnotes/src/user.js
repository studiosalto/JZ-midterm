import * as cookies from './cookies';

const USER = {
    name: 'Frank',

}

const cookieAccessToken = 'accessToken'

export function getUser() {
    return new Promise((resolve, reject) => {
        const accessToken = cookies.get(cookieAccessToken);
        if (accessToken) {
            resolve(USER);
        } else {
            reject();
        }
    })
}

export function login() {
    return new Promise((resolve, reject) => {
        cookies.set(cookieAccessToken, 'test');
        resolve();
    })
}