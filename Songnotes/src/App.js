import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';
import SpotifySearch from './SpotifySearch';
import {getUser, login} from './user';
import {PASTEVENTS, COMINGEVENTS} from "./data"


class App extends Component {
    constructor(props) {
        super(props);

        this.state = {
            user: undefined,
        }
    }

    getUser = () => {
        getUser().then((user) => {
            console.log('onsuccess')
            this.setState({
                user
            }, () => {
                console.log('failed')
            });
        })
    }

    componentDidMount() {
        this.getUser();
    }

    render() {
        const {user} = this.state;
        return (
            <div className="App">

                <div className="container">
                    {user ? (
                        <div>
                            <Navbar/>
                            <LoggedIn user={user}/>
                        </div>
                    ) : (
                        <Login onLogin={this.getUser()}/>
                    )}
                </div>

            </div>
        );
    }
}

class Navbar extends Component {
    render() {
        return (
            <div className="">
                <nav class="navbar navbar-expand-lg navbar-light bg-light justify-content-between">
                    <a class="navbar-brand" href="/">
                        <div className="App-title">
                            <img src="/logo.png" alt="Songnotes logo"/>
                        </div>
                    </a>
                    <div>
                        <ul class="navbar-nav flex-row ml-md-auto d-none d-md-flex">
                            <li class="nav-item">
                                <a class="nav-link p-2" href="https://twitter.com/getbootstrap" target="_blank" rel="noopener" aria-label="Twitter">
                                    Events

                                </a>
                            </li>
                            <li class="nav-item">
                                <a class="nav-link p-2" href="https://twitter.com/getbootstrap" target="_blank" rel="noopener" aria-label="Twitter">
                                    Create event

                                </a>
                            </li>
                            <li class="nav-item dropdown">
                                <a class="nav-item nav-link dropdown-toggle mr-md-2" href="#" id="bd-versions" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                                    Account
                                </a>
                                <div class="dropdown-menu dropdown-menu-right" aria-labelledby="bd-versions">
                                    <a class="dropdown-item active" href="/docs/4.0/">Profile</a>
                                    <a class="dropdown-item" href="https://v4-alpha.getbootstrap.com/">Logout</a>
                                </div>
                            </li>

                        </ul>
                    </div>

                </nav>
            </div>
        )
    }
}

class Login extends Component {
    constructor(props) {
        super(props);
    }

    submit = (e) => {
        e.preventDefault();
        console.log('submit')
        login(this.state).then(this.props.onLogin,)
    }

    onChange = (e) => {
        this.setState({
            [e.target.name]: e.target.value,
        });
    }


    render() {
        return (
            <div>
                <form onSubmit={this.submit}>
                    <input type="text" onChange={this.onChange}/>
                    <input type="password" onChange={this.onChange}/>
                    <button type="submit">Login</button>
                </form>
            </div>
        )
    }
}

class LoggedIn extends Component {
    render() {
        return (
            <div>
                <div style={{marginBottom: 20}}>
                    <h2>
                        Events
                    </h2>
                    <EventList past={true} events={PASTEVENTS}/>
                </div>
                <div>
                    <a href="/create">Create event</a>
                </div>
                <div>
                    <h2>
                        Past events
                    </h2>
                    <EventList events={COMINGEVENTS}/>
                </div>

            </div>
        );
    }
}

class EventList extends Component {
    render() {
        const events = this.props.events || [];
        let i = 0;
        return (
            events.map((event) => {
                i++;
                const date = new Date(this.props.past ? event.pastDate : event.futureDate).toString();

                return (
                    <div className="event" style={{marginBottom: 25}}>
                        <div style={{
                            float: 'left',
                            borderRadius: '100%',
                            marginRight: 20,
                            width: 50,
                            height: 50,
                            overflow: 'hidden',
                        }}>
                            <img src={`http://i.pravatar.cc/50?img=${i}`}/>
                        </div>
                        <h5>{event.name}</h5>
                        <h6>by: <bold>{event.organiser}</bold> at <bold>{date}</bold></h6>
                    </div>
                );
            })
        );
    }
}

export default App;
