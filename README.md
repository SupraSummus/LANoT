Environment for [Fanstel BT840x/USB840x modules](https://www.fanstel.com/bt840) using [nRF52840 SoC](https://www.nordicsemi.com/?sc_itemid=%7B2DC10BA5-A76E-40F8-836E-E2FC65803A71%7D).

Axioms:
 * Strong separation between kernel and user space to catch any error (segfault/sigill/hang/...) in user code and report it in civilized manner. Also, any error in code should not prevent system from booting to recoverable state.
 * Programmer included, exposed as OS service and reachable via wireless communication. Any user error in programming procedure should not prevent system from booting to recoverable state.
 * Wireless communication (or any other) included as OS module. Any error in user code shouldnt prevent system from establishing wireless communication.

In one sentence: deployed hardware device is reprogrammable by user, in a safe way, without physical access.

See also `usecase.pl.md` (currently only in polish).
