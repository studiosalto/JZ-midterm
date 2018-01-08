import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';
import {searchTracks} from './spotify';

const REQUEST_STATUS = {
    waiting: 'waiting',
    busy: 'busy',
    error: 'error',
}

class App extends Component {
    constructor(props) {
        super(props);

        this.state = {
            query: '',
            results: [],
            requestStatus: REQUEST_STATUS.waiting,
        }
    }

    searchSpotify = () => {
        searchTracks(this.state.query).then(
            (resolved) => {
                console.log(resolved);
            },
            () => {

            }
        )
    };

    onChange = (e) => {
        const value = e.target.value;
        this.setState({
            query: value,
        }, () => {
            this.searchSpotify()
        })
    };


    render() {
        return (
            <div className="spotify-search" style={{
                marginTop: 20,
            }}>
                <input onChange={this.onChange}/>

                {this.state.results.map((result) => {
                    return (
                        <div>
                            {result}
                        </div>
                    );
                })}
            </div>
        );
    }
}

export default App;
