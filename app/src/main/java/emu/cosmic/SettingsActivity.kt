package emu.cosmic

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContract
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.preference.Preference
import androidx.preference.PreferenceFragmentCompat
import emu.cosmic.data.CosmicSettings
import emu.cosmic.databinding.SettingsActivityBinding
import emu.cosmic.fragments.SettingsFragment

class SettingsActivity : AppCompatActivity(), PreferenceFragmentCompat.OnPreferenceStartFragmentCallback {
    private val binding by lazy {
        SettingsActivityBinding.inflate(layoutInflater)
    }
    private val preference by lazy { SettingsFragment() }
    private var fragmentName: String? = ""
    private var defaultBarTitle: String? = ""

    companion object {
        var filePicker: ActivityResultLauncher<CharSequence>? = null
        var threatPickerEvent: ((Pair<Uri?, String>) -> Unit)? = null
    }
    private val settings = CosmicSettings.globalSettings

    inner class DirectoryPickerContract : ActivityResultContract<CharSequence, Pair<Uri?, String>>() {
        private lateinit var holderName: String
        override fun createIntent(context: Context, input: CharSequence): Intent =
            Intent(Intent.ACTION_OPEN_DOCUMENT_TREE).apply {
                putExtra("EXTRA_INITIAL_URI", settings.appStorage)
                holderName = "$input"
            }
        override fun parseResult(resultCode: Int, intent: Intent?): Pair<Uri?, String> {
            return when (resultCode) {
                Activity.RESULT_OK -> Pair(intent?.data, holderName)
                else -> Pair(null, holderName)
            }
        }
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        defaultBarTitle = "${supportActionBar?.title}"
        filePicker = registerForActivityResult(DirectoryPickerContract()) { pair ->
            if (pair.first == null)
                return@registerForActivityResult

            contentResolver?.takePersistableUriPermission(
                pair.first!!, Intent.FLAG_GRANT_READ_URI_PERMISSION)
            threatPickerEvent?.invoke(pair)
        }
        // Add the back button, so the user can return to the main screen
        WindowCompat.setDecorFitsSystemWindows(window, false)
        binding.appToolBar.apply {
            title = resources.getString(R.string.toolbar_settings)
        }

        setSupportActionBar(binding.appToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        supportFragmentManager.beginTransaction()
            .replace(R.id.preferencesContainer, preference)
            .commit()
    }
    private fun changeAppBarTitle(prefer: Preference?) {
        prefer?.let {
            fragmentName = it.fragment
        }
        val titles = mapOf(
            Pair("emu.cosmic.fragments.GlobalSettingsFragment", "Global Settings"),
            Pair("emu.cosmic.fragments.GraphicsSettingsFragment", "Graphics Settings")
        )
        supportActionBar?.title = titles[fragmentName] ?: defaultBarTitle
    }
    override fun onPreferenceStartFragment(caller: PreferenceFragmentCompat, pref: Preference): Boolean {
        val fragment = supportFragmentManager.fragmentFactory.instantiate(classLoader, pref.fragment!!)
        fragment.arguments = pref.extras

        // Replace the existing Fragment with the new Fragment
        changeAppBarTitle(pref)
        supportFragmentManager.beginTransaction()
            .replace(R.id.preferencesContainer, fragment)
            .addToBackStack(null)
            .commit()
        return true
    }
}